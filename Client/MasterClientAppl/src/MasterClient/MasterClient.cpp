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

    /*Master client has to authenticate instead of login*/
    disconnect(cs, SIGNAL(connectedToCmdServer()),(Client*) this,SLOT(login()));
    connect(cs, SIGNAL(connectedToCmdServer()),this,SLOT(authenticate()));

    connect(cs, SIGNAL(lostConnection()),this,SLOT(connectionLostMaster()));



    srand(123);
    char buf[64];
    sprintf(buf, "%d", rand());
    nonce1 = QString(buf);
    msgAuth = new MessageAuthenticator();
    auth_state = AUTH_IDLE;
    sym_key = "KATZE";

    id = "master";

    ranf_queue = new RedeanfrageQueue();
    ranf_mute = false;

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
            /***
             *  USE SESSION KEY !!!!!!!
             *
             */
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
    return NULL;
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
    ranf_queue -> clear();
}

void
MasterClient::muteRanf()
{
    ranf_mute = !ranf_mute;
    if (ranf_mute)
        registerdFunctions.remove(CMD_RANF_ASK);
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
    Redeanfrage *ranf = ranf_queue->dequeue();
    if (ranf != NULL)
        delete ranf;
}
