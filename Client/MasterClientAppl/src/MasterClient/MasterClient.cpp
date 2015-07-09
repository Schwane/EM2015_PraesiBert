/*
 * MasterClient.cpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#include "MasterClient.hpp"

MasterClient::MasterClient()
{
    registerdFunctions.insert(CMD_AUTH_PHASE2,static_cast<remoteFunction>(&MasterClient::loginResponse));
    registerdFunctions.insert(CMD_RANF_ASK,static_cast<remoteFunction>(&MasterClient::redeanfrage));
    registerdFunctions.insert(CMD_RANF_RE_RESP,static_cast<remoteFunction>(&MasterClient::redeanfrageFinal));

    /*Master client has to authenticate instead of login*/
    disconnect(cs, SIGNAL(connectedToCmdServer()),(Client*) this,SLOT(login()));
    connect(cs, SIGNAL(connectedToCmdServer()),this,SLOT(authenticate()));

    connect(cs, SIGNAL(lostConnection()),this,SLOT(connectionLostMaster()));



    srand(123);
    char buf[64];
    sprintf(buf, "%016X", rand());
    nonce1 = QString(buf);
    msgAuth = new MessageAuthenticator();
    auth_state = AUTH_IDLE;
    sym_key = NULL;

    id = "master";

    ranf_queue = new RedeanfrageQueue();
    ranf_mute = false;
    current_ranf = NULL;
    connect(ranf_queue, SIGNAL(sizeChanged(int)), this, SIGNAL(ranfSizeChanged(int)));
}

MasterClient::~MasterClient()
{
    delete msgAuth;
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
            nonce2 = parameters.value("nonce2").toString();
            QByteArray cattedNonces;

            cattedNonces.append(nonce1);
            cattedNonces.append(nonce2);
            mac_key = msgAuth->hmacSha1(sym_key,cattedNonces);
            msgAuth -> setKey(mac_key);
            disconnect(xmlmw, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendCmd(QByteArray)));
            connect(xmlmw, SIGNAL(messageWritten(QByteArray)), msgAuth, SLOT(authenticateMessage(QByteArray)));
            connect(msgAuth, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendCmd(QByteArray)));
            auth_state = AUTH_PROOF;
        }
        else
        {
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
                auth_state = AUTH_ACCEPTED;
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
    if (parameters.contains("clientId") && parameter_types.contains("clientId") && parameter_types.value("clientId") == "string")
    {
         Redeanfrage *ranf = new Redeanfrage(parameters.value("clientId").toString());
         ranf -> queue();
         ranf_queue->enqueue(ranf);
         resp -> addParameter("status", QString("ok"));
    }
    else
    {
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
    if (parameters.contains("status") && parameter_types.contains("status") && parameter_types.value("status") == "string" && parameters.value("status").toString() == "ACCEPTED")
    {
         resp -> addParameter("status", QString("ok"));
         answ = "ACCEPTED";
    }
    else
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Redenafrage rejected"));
        answ = "REJECTED";
    }
    if (current_ranf != NULL && answ == "REJECTED")
    {
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
    msg->addParameter("nonce1", nonce1);
    xmlmw -> writeMessage(msg);
    auth_state = AUTH_RCV_NONCE;
    login_state = TRYING;
    emit loginStateChanged();
}

void
MasterClient::connectionLostMaster()
{
    auth_state = AUTH_IDLE;

    connect(xmlmw, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendCmd(QByteArray)));
    disconnect(xmlmw, SIGNAL(messageWritten(QByteArray)), msgAuth, SLOT(authenticateMessage(QByteArray)));
    disconnect(msgAuth, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendCmd(QByteArray)));
}


void
MasterClient::clearRanf(){
    Message *msg;
    for (int i = 0; i < ranf_queue->getSize(); i++)
    {
        msg = new Message(CMD_RANF_RESP, id, ranf_queue->getClientIdAt(i));
        msg->addParameter("status", QString("REJECTED"));
        xmlmw->writeMessage(msg);
        delete msg;
    }
    ranf_queue -> clear();
}

void
MasterClient::muteRanf()
{
    ranf_mute = !ranf_mute;
    registerdFunctions.remove(CMD_RANF_ASK);
    if (ranf_mute)
        registerdFunctions.insert(CMD_RANF_ASK, static_cast<remoteFunction>(&MasterClient::redeanfrageAutoReject));
    else
        registerdFunctions.insert(CMD_RANF_ASK, static_cast<remoteFunction>(&MasterClient::redeanfrage));
    emit ranfMuteChanged(ranf_mute);
}

void
MasterClient::dummyRanf()
{
    Redeanfrage *ranf1 = new Redeanfrage("xxx" + rand());
    Redeanfrage *ranf2 = new Redeanfrage("yyy" + rand());
    ranf_queue->enqueue(ranf1);
    ranf_queue->enqueue(ranf2);
}

void
MasterClient::acceptRanf()
{
    if (current_ranf == NULL)
    {
        current_ranf = ranf_queue->dequeue();
        if (current_ranf != NULL)
        {
            Message *msg = new Message(CMD_RANF_RESP, id, current_ranf->getClientId());
            msg->addParameter("status", QString("ACCEPTED"));
            xmlmw->writeMessage(msg);
        }
    }

}


void
MasterClient::finishRanf()
{
    if (current_ranf != NULL)
    {
        Message *msg = new Message(CMD_RANF_FINISH, id, current_ranf->getClientId());
        delete current_ranf;
        current_ranf = NULL;
        xmlmw->writeMessage(msg);
    }

}

void
MasterClient::setKey(QString key)
{
    QByteArray newKey;
    newKey.append(key);
    this-> sym_key = newKey;
}
