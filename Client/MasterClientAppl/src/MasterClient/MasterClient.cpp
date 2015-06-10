/*
 * MasterClient.cpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#include "MasterClient.hpp"

MasterClient::MasterClient()
{
    registerdFunctions.insert("login",static_cast<remoteFunction>(&MasterClient::loginResponse));

    disconnect(cs, SIGNAL(connected()),this,SLOT(login()));
    connect(cs, SIGNAL(connected()),this,SLOT(authenticate()));

    //disconnect(cs, SIGNAL(disconnected()),this,SLOT(connectionLost()()));
    connect(cs, SIGNAL(disconnected()),this,SLOT(connectionLostMaster()));


    srand(123);
    char buf[64];
    sprintf(buf, "%d", rand());
    nonce1 = QString(buf);
    msgAuth = new MessageAuthenticator();
    auth_state = AUTH_IDLE;
    sym_key = "KATZE";
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
        resp = new Message("PROOF_RESPONSE","client","server");
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
            msgAuth -> setKey(sym_key);
            disconnect(xmlmw, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendData(QByteArray)));
            connect(xmlmw, SIGNAL(messageWritten(QByteArray)), msgAuth, SLOT(authenticateMessage(QByteArray)));
            connect(msgAuth, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendData(QByteArray)));
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
        resp = new Message("LOGIN_RESPONSE","client","server");
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
        resp = new Message("RESPONSE","client","server");
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: authentication failed - unknown reason"));
        connectionLost();
    }
    return resp;
}

void
MasterClient::authenticate()
{
    Message* msg = new Message("LOGIN_NONCE", "client", "server");
    msg->addParameter("nonce", nonce1);
    xmlmw -> writeMessage(msg);
    auth_state = AUTH_RCV_NONCE;
    login_state = TRYING;
    emit loginStateChanged();
}

void
MasterClient::connectionLostMaster()
{
    auth_state = AUTH_IDLE;

    connect(xmlmw, SIGNAL(messageWritten(QByteArray)), cs, SLOT(sendData(QByteArray)));
    disconnect(xmlmw, SIGNAL(messageWritten(QByteArray)), msgAuth, SLOT(authenticateMessage(QByteArray)));
    disconnect(msgAuth, SIGNAL(messageAuthenticated(QByteArray)),cs, SLOT(sendData(QByteArray)));
}
