/*
 * Listener.cpp
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#include <Qt>

#include <src/backend/Listener.h>

#include <commands.hpp>

#include <src/backend/Logger.h>
#include <src/backend/UnspecifiedClient.h>

namespace ServerAppl
{
    Listener::Listener()
    {
    }

    Listener::Listener(UnspecifiedClient * priorClientObject)
    {
        WRITE_DEBUG("Listener constructor called.")
        this->priorClientObject = priorClientObject;
        this->clientId = priorClientObject->getClientId();
        this->name = priorClientObject->getName();
        this->lastTimestamp = priorClientObject->getLastTimestamp();
        this->hasPresentation = FALSE;
        this->loginAcknowledged = FALSE;

        WRITE_DEBUG("Listener constructor finished.")
    }

    Listener::~Listener()
    {
        // TODO Auto-generated destructor stub
    }

    Listener*  Listener::createListener(UnspecifiedClient* client)
    {
        Listener* newListener;
        WRITE_DEBUG("createListener called.")
        newListener = new Listener(client);
        if(newListener)
        {
            WRITE_DEBUG("createListener: newListener created.")
        }
        else
        {
            WRITE_DEBUG("createListener: newListener is NULL.")
        }
        return newListener;
    }


//    Message* Listener::handleReceivedMessage(QString commandName, Message* msg)
//    {
//        Message * responseMessage;
//
//        if("login" == commandName)
//        {
//            /* command is login (compare returns 0) */
//            responseMessage = new Message("login", "server", "client");
//
//            if(connectionStm(Login))
//            {
//                responseMessage->addParameter(QString("status"), QString("ok"));
//            }
//            else
//            {
//                responseMessage->addParameter(QString("status"), QString("rejected"));
//            }
//        }
//
//        delete msg;
//
//        return responseMessage;
//    }

    Message* Listener::handleUnknownMessage(QString commandName, Message* msg)
    {
        Message * responseMessage = NULL;
        bool uIntConversionSuccessfull = FALSE;
        QString receiver = msg->getReceiver();
        unsigned int senderId = 0;

        senderId = msg->getReceiver().toUInt(&uIntConversionSuccessfull, 10);

        if("master" == msg->getReceiver()
                && uIntConversionSuccessfull
                && this->getClientId() == senderId
                )
        {
            emit forwaredMessageToMaster(msg, senderId);
        }
        else
        {
            delete(msg);
        }

        return NULL;
    }

    Message* Listener::handleReceivedAudio(QString commandName, Message* msg)
    {

        if(IS_COMMAND(DATA_AUDIO, commandName)
                && msg->getParameters()->contains("audio")
                )
        {
            QByteArray audioRecording = QByteArray::fromBase64(msg->getParameters()->value("audio").toByteArray());
            QString fileName;
            QString listenerId;

            listenerId.setNum(this->clientId);

            fileName.clear();
            fileName.append("ranf_client_id_");
            fileName.append(listenerId);
            fileName.append("_at_");
            fileName.append(msg->getTimestamp().toString(Qt::ISODate));
            fileName.append(".m4a");

            emit writeAudioRecording(fileName, audioRecording);

            delete(msg);
        }

        return NULL;
    }

    void Listener::setHasPresentation(bool hasPresentation)
    {
        this->hasPresentation = hasPresentation;
    }

    bool Listener::getHasPresentation()
    {
        return this->hasPresentation;
    }

    ClientType Listener::getClientType()
    {
        return ClientType_Listener;
    }

    Message* Listener::handleAcknowledge(QString commandName, Message* msg)
    {
        Message * responseMessage = NULL;

        if(IS_COMMAND(commandName, CMD_ACK_RESPONSE))
        {
            if(!this->loginAcknowledged && !this->hasPresentation)
            {
                this->loginAcknowledged = TRUE;
                this->hasPresentation = TRUE;

                emit requestDeliverPresentation(this->clientId);
            }
        }

        delete msg;

        return responseMessage;
    }
} /* namespace ServerAppl */
