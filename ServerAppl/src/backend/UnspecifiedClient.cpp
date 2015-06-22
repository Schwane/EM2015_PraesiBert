/*
 * Client.cpp
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#include <src/backend/UnspecifiedClient.h>

#include <Message.hpp>

#include <src/backend/Server.h>

namespace ServerAppl
{
    UnspecifiedClient::UnspecifiedClient()
    {
        clientId = 0;
    }

    UnspecifiedClient::UnspecifiedClient(Server * server, uint clientId, QString name)
    {
        this->server = server;
        this->clientId = clientId;
        this->name = name;
        lastTimestamp = QTime();
    }

    UnspecifiedClient::~UnspecifiedClient()
    {
    }

    Message* UnspecifiedClient::handleReceivedMessage(QString commandName, Message* msg)
    {
        Message * responseMessage;

        if(IS_COMMAND(commandName, "login"))
        {
            responseMessage = handleLoginMessages(commandName, msg);
        }
        else if(IS_COMMAND(commandName, "LOGIN_NONCE"))
        {
            responseMessage = handleLoginMessages(commandName, msg);
        }
        else
        {
            responseMessage = new Message(QString("unknown_message"),QString("server"), QString("client"));
            responseMessage->addParameter(QString("receivedCommand"), commandName);
        }

        return responseMessage;
    }

    Message* UnspecifiedClient::handleLoginMessages(QString commandName, Message* msg)
    {
        Message * responseMessage;

        if(IS_COMMAND(commandName, "login"))
        {
            Listener * newListener;

            responseMessage = new Message(QString("login_RESPONSE"), QString("server"), QString("client"));

            if(Listener::createListener(this, newListener))
            {
                if(this->server->registerListener(newListener))
                {
                    responseMessage->addParameter(QString("status"), QString("ok"));
                }
                else
                {
                    delete(newListener);
                    responseMessage->addParameter(QString("status"), QString("rejected"));
                }
            }

        }
        else if(IS_COMMAND(commandName, "LOGIN_NONCE"))
        {
            Master * newMaster;
            const QMap<QString, QVariant>* parameters = msg->getParameters();

            responseMessage = new Message(QString("login_RESPONSE"), QString("server"), QString("client"));

            if(parameters->contains(QString("nonce")))
            {
                QString nonce1 = parameters->value(QString("nonce")).toString();

                if(Master::createMaster(this, newMaster, nonce1))
                {
                    if(this->server->registerMaster(newMaster))
                    {
                        responseMessage->addParameter(QString("nonce2"), QString("nonce_2"));
                    }
                    else
                    {
                        delete(newMaster);
                        responseMessage->addParameter(QString("status"), QString("rejected"));
                    }
                }
                else
                {
                    responseMessage->addParameter(QString("status"), QString("rejected"));
                }
            }
            else
            {
                responseMessage->addParameter(QString("status"), QString("rejected"));
            }
        }
        else
        {
            responseMessage = new Message(QString("unknown_message"),QString("server"), QString("client"));
            responseMessage->addParameter(QString("receivedCommand"), commandName);
        }

        delete(msg);

        return responseMessage;
    }

    unsigned int UnspecifiedClient::getClientId()
    {
        return clientId;
    }

    QString ServerAppl::UnspecifiedClient::getName()
    {
        return name;
    }

    QTime ServerAppl::UnspecifiedClient::getLastTimestamp()
    {
        return lastTimestamp;
    }

    Server* ServerAppl::UnspecifiedClient::getServer()
    {
        return server;
    }
} /* namespace ServerAppl */
