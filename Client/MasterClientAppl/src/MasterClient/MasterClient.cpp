/*
 * MasterClient.cpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#include "MasterClient.hpp"

MasterClient::MasterClient()
{
    //register remote functions
    registerdFunctions.insert(CMD_AUTH_PHASE2,static_cast<remoteFunction>(&MasterClient::loginResponse));
    registerdFunctions.insert(CMD_AUTH_PHASE4,static_cast<remoteFunction>(&MasterClient::loginResponse));
    registerdFunctions.insert(CMD_RANF_ASK,static_cast<remoteFunction>(&MasterClient::redeanfrage));
    registerdFunctions.insert(CMD_RANF_RE_RESP,static_cast<remoteFunction>(&MasterClient::redeanfrageFinal));

    /*Master client has to authenticate instead of login, so rearange signals*/
    disconnect(cs, SIGNAL(connectedToCmdServer()),(Client*) this,SLOT(login()));
    connect(cs, SIGNAL(connectedToCmdServer()),this,SLOT(authenticate()));
    connect(cs, SIGNAL(lostConnection()),this,SLOT(connectionLostMaster()));
    connect(prs, SIGNAL(isRunning(bool)), this, SIGNAL(praesentationRunning(bool)));

    /*create random number*/
    //@ TODO Use seed with sufficient entropy, NOT fixed value. Is not in scope of this course.
    srand(123);
    char buf[64];
    sprintf(buf, "%016X", rand());
    nonce1 = QString(buf); //save the nonce

    //create authenticators for data and command
    msgAuthCmd = new MessageAuthenticator();
    msgAuthData = new MessageAuthenticator();

    //set authentication state and key
    auth_state = AUTH_IDLE;
    sym_key = NULL;

    //id is always master
    id = "master";

    //initialize talk-request queue
    ranf_queue = new RedeanfrageQueue();
    ranf_mute = false;
    ranf_size = 0;
    current_ranf = NULL;
    connect(ranf_queue, SIGNAL(sizeChanged(int)), this, SIGNAL(ranfSizeChanged(int)));

    //initialize camera controller
    cc = new CameraController(this);
    connect(cc, SIGNAL(gestureDetected(int)), this, SLOT(requestSlideChange(int)));


}

MasterClient::~MasterClient()
{
    delete msgAuthCmd;
    delete msgAuthData;
}

Message*
MasterClient::loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp;
    if (auth_state == AUTH_RCV_NONCE)
    {
        resp = new Message(CMD_AUTH_PHASE3,id,"server");
        if (parameters.contains("nonce2"))
        {
            nonce2 = parameters.value("nonce2").toString(); //get second nonce
            QByteArray cattedNonces;

            cattedNonces.append(nonce1);
            cattedNonces.append(nonce2);
            mac_key = msgAuthCmd->hmacSha1(sym_key,cattedNonces); //derive session key from catted nonces

            msgAuthCmd -> setKey(mac_key);
            msgAuthData -> setKey(mac_key);

            disconnect(xmlmw, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendCmd(QByteArray))); //use authenticators now by rearanging signals
            connect(xmlmw, SIGNAL(messageWritten(QByteArray)), msgAuthCmd, SLOT(authenticateMessage(QByteArray)));
            connect(msgAuthCmd, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendCmd(QByteArray)));

            disconnect(xmlmw_data, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendData(QByteArray)));
            connect(xmlmw_data, SIGNAL(messageWritten(QByteArray)), msgAuthData, SLOT(authenticateMessage(QByteArray)));
            connect(msgAuthData, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendData(QByteArray)));
            //note that thereby the proof is sent automatically

            auth_state = AUTH_PROOF; // set state
        }
        else
        {
            // on error
            resp -> addParameter("status",QString("error"));
            resp -> addParameter("message",QString("Parameter: nonce2 - no nonce2 delivered"));
            auth_state = AUTH_IDLE;
            login_state = REJECTED;
            emit loginStateChanged();
        }
    }
    else if (auth_state == AUTH_PROOF)
    {
        resp = new Message(CMD_ACK_RESPONSE,"client","server");
        if (parameters.contains("status"))
        {
            QString status = parameters.value("status").toString();
            if (status == "ok")
            {
                auth_state = AUTH_ACCEPTED; // accept if server accepted formally
                login_state = ACCEPTED;
            }
            else
            {
                auth_state = AUTH_REJECTED;
                login_state = REJECTED;
            }
            emit loginStateChanged();
            resp -> addParameter("status", QString("ok"));
        }
        else
        {
            resp -> addParameter("status",QString("error"));
            resp -> addParameter("message",QString("Parameter: authentication failed - server rejected"));
        }
    }
    else
    {
        resp = new Message(CMD_ACK_RESPONSE,"client","server");
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: authentication failed - unknown reason"));
        connectionLost();
    }
    return resp;
}

Message*
MasterClient::redeanfrage(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp;
    resp = new Message("ACK",id,"server");
    //check parameters
    if (parameters.contains("clientId") && parameter_types.contains("clientId") && parameter_types.value("clientId") == "string")
    {
         Redeanfrage *ranf = new Redeanfrage(parameters.value("clientId").toString());
         ranf -> queue(); //enqueue talk-request
         ranf_queue->enqueue(ranf);
         resp -> addParameter("status", QString("ok"));
    }
    else
    {
        //on error
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: clientId as string missing"));
    }
    return resp;
}

Message*
MasterClient::redeanfrageAutoReject(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp;
    if (parameters.contains("clientId") && parameter_types.contains("clientId") && parameter_types.value("clientId") == "string")
    {
        //always reject talk-requests
        resp = new Message(CMD_RANF_RESP, "master", parameters.value("clientId").toString());
        resp -> addParameter("status", QString("REJECTED"));
    }
    else
    {
        resp = new Message(CMD_ACK_RESPONSE, "master", "client");
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: clientId as string missing"));
    }
    return resp;
}

Message*
MasterClient::redeanfrageFinal(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp;
    resp = new Message("ACK",id,"server");
    QString answ;
    //check parameters
    if (parameters.contains("status") && parameter_types.contains("status") && parameter_types.value("status") == "string" && parameters.value("status").toString() == "ACCEPTED")
    {
         resp -> addParameter("status", QString("ok")); //if request was relevant
         answ = "ACCEPTED";
    }
    else
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Redenafrage rejected"));
        answ = "REJECTED"; //if it is not relevant anymore
    }
    if (current_ranf != NULL && answ == "REJECTED")
    {
        //delete the reqeust if it was not relevant anymore
        delete current_ranf;
        current_ranf = NULL;
    }
    emit ranfFinalAnswer(answ);
    return resp;
}



void
MasterClient::authenticate()
{
    login_state = CONNECTED;
    emit loginStateChanged();
    Message* msg = new Message(CMD_AUTH_PHASE1, "client", "server");
    msg->addParameter("nonce1", nonce1);//send nonce
    xmlmw -> writeMessage(msg);
    auth_state = AUTH_RCV_NONCE;//set states
    login_state = TRYING;
    emit loginStateChanged();
}

void
MasterClient::connectionLostMaster()
{
    if (auth_state != AUTH_IDLE && auth_state != AUTH_RCV_NONCE)
    {
        auth_state = AUTH_IDLE;

        //rearange signals to initial setup, beacuse we are not authenticated anymore

        connect(xmlmw, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendCmd(QByteArray)));
        disconnect(xmlmw, SIGNAL(messageWritten(QByteArray)), msgAuthCmd, SLOT(authenticateMessage(QByteArray)));
        disconnect(msgAuthCmd, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendCmd(QByteArray)));

        connect(xmlmw_data, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendData(QByteArray)));
        disconnect(xmlmw_data, SIGNAL(messageWritten(QByteArray)), msgAuthData, SLOT(authenticateMessage(QByteArray)));
        disconnect(msgAuthData, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendData(QByteArray)));
    }
}


void
MasterClient::clearRanf(){
    //clear the whole queue
    Message *msg;
    for (int i = 0; i < ranf_queue->getSize(); i++)
    {
        msg = new Message(CMD_RANF_RESP, id, ranf_queue->getClientIdAt(i)); //for each request send a reject
        msg->addParameter("status", QString("REJECTED"));
        xmlmw->writeMessage(msg);
    }
    ranf_queue -> clear(); //clear the queue
    delete current_ranf; //clear current question
    current_ranf = NULL;
}

void
MasterClient::muteRanf()
{
    ranf_mute = !ranf_mute;
    registerdFunctions.remove(CMD_RANF_ASK);
    //if muted, reconnect signals to auto reject or vice versa
    if (ranf_mute)
        registerdFunctions.insert(CMD_RANF_ASK, static_cast<remoteFunction>(&MasterClient::redeanfrageAutoReject));
    else
        registerdFunctions.insert(CMD_RANF_ASK, static_cast<remoteFunction>(&MasterClient::redeanfrage));
    emit ranfMuteChanged(ranf_mute);
}

void
MasterClient::acceptRanf()
{
    //if no current request
    if (current_ranf == NULL)
    {
        current_ranf = ranf_queue->dequeue(); //get one
        if (current_ranf != NULL) //if we got one
        {
            Message *msg = new Message(CMD_RANF_RESP, id, current_ranf->getClientId());
            msg->addParameter("status", QString("ACCEPTED")); //accept it
            xmlmw->writeMessage(msg);
        }
    }

}


void
MasterClient::finishRanf()
{
    if (current_ranf != NULL) //if there is a current request
    {
        Message *msg = new Message(CMD_RANF_FINISH, id, current_ranf->getClientId()); //finish it
        delete current_ranf;//delete it
        current_ranf = NULL;
        xmlmw->writeMessage(msg);
    }

}

void
MasterClient::setKey(QString key)
{
    //set the session key
    QByteArray newKey;
    newKey.append(key);
    this-> sym_key = newKey;
}

void
MasterClient::selectPraesentation(QString path)
{
    //read presentation from path
    prs -> reset();
    char slidename[8];
    int slide = 0;

    QList<QString> supported_exts = QList<QString>() << "jpg" << "JPG"; //suported fromats


    QString current_path;
    QFile F(current_path);

    bool proceed = false;

    while (1)
    {
        sprintf(slidename, "%03d.", slide); //set filename consecutively statring by 0
        for (int i = 0; i < supported_exts.size(); i++)
        {
            current_path = path;
            current_path.append("/");
            current_path.append(slidename);
            current_path.append(supported_exts.at(i)); //create path
            F.setFileName(current_path);
            if (F.exists()) //if exists, append it to presentation
            {
                qDebug() << "slide found: " << current_path << endl;
                proceed = true;
                break;
            }
        }
        if (!proceed) //last slide reached
            break;
        prs->appendSlide(current_path);
        slide++;
        proceed = false;
    }
}

void
MasterClient::deliverPraesentation()
{
    Message *msg = prs->packPraesentation(); //send presentation to server
    xmlmw_data -> writeMessage(msg);
}

void
MasterClient::requestStopPraesentation()
{
    Message *msg = new Message(CMD_STOP_PRAESENTATION, id, "server"); //request a stop
    xmlmw->writeMessage(msg);
}

void
MasterClient::activateGesture(bool active)
{
    //activate or deactivate gesture control
    if(active)
        cc -> start();
    else
        cc -> stop();

}
