/*
 * Client.cpp
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#include <src/backend/UnspecifiedClient.h>

#include <commands.hpp>
#include <Message.hpp>

#include <src/backend/Logger.h>
#include <src/backend/Master.h>
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

        responseMessage = new Message(QString("unknown_message"),QString("server"), QString("client"));
        responseMessage->addParameter(QString("receivedCommand"), commandName);

        delete(msg);

        return responseMessage;
    }

    Message* UnspecifiedClient::handleLoginMessages(QString commandName, Message* msg)
    {
        Message * responseMessage;

        if(IS_COMMAND(commandName, CMD_LOGIN))
        {
            Listener * newListener = Listener::createListener(this);
            responseMessage = new Message(CMD_LOGIN_RESP, QString("server"), QString("client"));

            if(newListener)
            {
                if(this->server->registerListener(newListener))
                {
                    WRITE_DEBUG("New Listener successfully registered.")
                    responseMessage->addParameter("status", QString("ok"));
                    responseMessage->addParameter("id", QString(newListener->getClientId()) );
                }
                else
                {
                    WRITE_DEBUG("Registration failed.")
                    delete(newListener);
                    responseMessage->addParameter(QString("status"), QString("rejected"));
                }
            }
            else
            {
                WRITE_DEBUG("NewListener was not created due to some reason.")
            }
        }
        else
        {
            responseMessage = new Message(QString(CMD_UNKNOWN),QString("server"), QString("client"));
            responseMessage->addParameter(QString("receivedCommand"), commandName);
        }

        delete(msg);

        return responseMessage;
    }

    Message* UnspecifiedClient::handleAuthPhase1(QString commandName, Message * msg)
    {
        Message * responseMessage;

        if(IS_COMMAND(commandName, CMD_AUTH_PHASE1))
        {
            const QMap<QString, QVariant>* parameters = msg->getParameters();

            WRITE_DEBUG("handle auth phase 1")

            responseMessage = new Message(CMD_AUTH_PHASE2, QString("server"), QString("client"));

            if(parameters->contains(QString("nonce1")))
            {
                QString nonce1 = parameters->value(QString("nonce1")).toString();
                Master * newMaster = Master::createMaster(this, nonce1);

                if( newMaster )
                {
                    if(this->server->registerMaster(newMaster))
                    {
                        NONCE nonce = newMaster->getNonce();
                        responseMessage->addParameter(QString("nonce2"), nonce.part_2);
                        newMaster->authenticationStm(TransmittedPhase2Message);
                        WRITE_DEBUG(responseMessage->getParameters()->value(QString("nonce2")).toString())
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
        WRITE_DEBUG(responseMessage->getCommand())
        return responseMessage;
    }

    ClientType UnspecifiedClient::getClientType()
    {
        return ClientType_Unspecified;
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
