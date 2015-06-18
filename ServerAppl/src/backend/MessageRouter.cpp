/*
 * MessageRouter.cpp
 *
 *  Created on: 16.06.2015
 *      Author: sebastian
 */

#include <src/backend/Logger.h>
#include <src/backend/MessageRouter.h>

namespace ServerAppl
{

    MessageRouter::MessageRouter()
    {
        registeredMessageHandlers = new QMap<uint, QMap<QString, messageHandler>* >();

    }

    MessageRouter::~MessageRouter()
    {
        int i = registeredMessageHandlers->count() - 1;

        while(0 <= i)
        {
            delete(registeredMessageHandlers->value(i));
            i--;
        }
        delete(registeredMessageHandlers);
    }

    bool MessageRouter::registerMessageHandler(uint clientId, QString command, MessageHandlerInterface * object,
            handleReceivedMessageFunction function)
    {
        messageHandler handler;

        handler.object = object;
        handler.function = function;

        return registerMessageHandler(clientId, command, handler);
    }

    bool MessageRouter::registerMessageHandler(uint clientId, QString command, messageHandler handler)
    {
        bool registeredMessageHandlerSuccessfull = FALSE;
        QMap<QString, messageHandler> * clientCommands;

        if(!registeredMessageHandlers->contains(clientId))
        {
            clientCommands = new QMap<QString, messageHandler>();
            registeredMessageHandlers->insert(clientId, clientCommands);
        }
        else
        {
            clientCommands = registeredMessageHandlers->value(clientId);
        }

        if(!clientCommands->contains(command))
        {
            clientCommands->insert(command, handler);
            registeredMessageHandlerSuccessfull = TRUE;
        }

        return registeredMessageHandlerSuccessfull;
    }

    bool MessageRouter::unregisterMessageHandler(uint clientId, QString command)
    {
        bool unregisteredMessageHandlerSuccessfull = FALSE;
        QMap<QString, messageHandler> * clientCommands;

        if(registeredMessageHandlers->contains(clientId))
        {
            clientCommands = registeredMessageHandlers->value(clientId);

            if(clientCommands->contains(command))
            {
                clientCommands->remove(command);
                unregisteredMessageHandlerSuccessfull = TRUE;
            }
        }
        return unregisteredMessageHandlerSuccessfull;
    }

    void MessageRouter::onMessageParsed(Message* message, uint clientId)
    {
        QString command = message->getCommand();
        QMap<QString, messageHandler> * clientCommands;
        Message * responseMessage;

        if(registeredMessageHandlers->contains(clientId))
        {
            clientCommands = registeredMessageHandlers->value(clientId);

            if(clientCommands->contains(command))
            {
                messageHandler handlerFunction = clientCommands->value(command);
                responseMessage = ((handlerFunction.object)->*(handlerFunction.function))(command, message);
                WRITE_DEBUG("MessageRouter: Handled received message.")
            }
            else
            {
                responseMessage = new Message(QString("RESPONSE"), message->getReceiver(), message->getSender());
                responseMessage->addParameter(QString("status"), QString("unknown command"));
                WRITE_DEBUG("MessageRouter: Received unknown message.")
            }
        }
        else
        {
            //TODO: client not registered. Maybe thrown an exception or emit error-signal?
        }

        if(!responseMessage)
        {
            if(message)
            {
                responseMessage = new Message(QString("RESPONSE"), message->getReceiver(), message->getSender());
            }
            else
            {
                responseMessage = new Message(QString("RESPONSE"), "", "server");
            }

            responseMessage->addParameter(QString("status"), QString("unknown server error"));
            WRITE_DEBUG("MessageRouter: Got Null-Pointer for response message.")
        }

        emit writeMessage(responseMessage, clientId);
    }

} /* namespace ServerAppl */
