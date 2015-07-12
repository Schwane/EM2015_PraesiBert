/*
 * Listener.cpp
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#include <src/backend/Listener.h>

#include <src/backend/Logger.h>
#include <src/backend/UnspecifiedClient.h>

namespace ServerAppl
{
    Listener::Listener()
    {
        connectionStm(Init);
    }

    Listener::Listener(UnspecifiedClient * priorClientObject)
    {
        WRITE_DEBUG("Listener constructor called.")
        this->priorClientObject = priorClientObject;
        this->clientId = priorClientObject->getClientId();
        this->name = priorClientObject->getName();
        this->lastTimestamp = priorClientObject->getLastTimestamp();
        this->hasPresentation = FALSE;
        connectionStm(Init);
//        delete(this->priorClientObject);
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

    Message* Listener::handleReceivedMessage(QString commandName, Message* msg)
    {
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
//    Message* Listener::handleLoginAcknowledge(QString commandName, Message* msg)
//    {
//        Message * responseMessage = NULL;
//
//        if(IS_COMMAND(commandName, CMD_LOGIN_ACK))
//        {
//            if(connectionStm(Login))
//            {
//                delete(this->priorClientObject);
//            }
//        }
//
//        delete msg;
//
//        return responseMessage;
//    }

    bool Listener::connectionStm(ListenerConnectionStmEvent event)
    {
        static ListenerConnectionStmState currentState = NotInitialized;
        ListenerConnectionStmState newState = NotInitialized;
        bool eventAllowed = FALSE;

        switch(currentState)
        {
            case NetworkConnectionEstablished:
                if(Login == event)
                {
                    newState = WaitingForSlides;
                    eventAllowed = TRUE;
                }
                break;
            case WaitingForSlides:
                if(StartTransmittingSlides == event)
                {
                    newState = ReceivingSlides;
                    eventAllowed = TRUE;
                }
                break;
            case ReceivingSlides:
                if(ReceivedSlides == event)
                {
                    newState = Ready;
                    eventAllowed = TRUE;
                }
                break;
            case Ready:
                if(ShowOtherSlide == event)
                {
                    newState = Ready;
                    eventAllowed = TRUE;
                }
                break;
            case NotInitialized:
                if(Init == event)
                {
                    newState = NetworkConnectionEstablished;
                    eventAllowed = TRUE;
                }
                break;
            default:
                break;
        }

        if(eventAllowed)
        {
            switch(newState)
            {
                case NetworkConnectionEstablished:
                    currentState = newState;
                    break;
                case WaitingForSlides:
                    currentState = newState;
                    break;
                case ReceivingSlides:
                    currentState = newState;
                    break;
                case Ready:
                    currentState = newState;
                    break;
                case NotInitialized:
                    currentState = newState;
                    break;
                default:
                    eventAllowed = FALSE;
                    break;
            }

        }

        return eventAllowed;
    }
} /* namespace ServerAppl */
