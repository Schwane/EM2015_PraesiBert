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
        registeredMessageHandlers = new QMap<QString, messageHandler>();

    }

    MessageRouter::~MessageRouter()
    {
        delete(registeredMessageHandlers);
    }

    bool MessageRouter::registerMessageHandler(QString command, MessageHandlerInterface * object,
            handleReceivedMessageFunction function)
    {
        messageHandler handler;

        handler.object = object;
        handler.function = function;

        return registerMessageHandler(command, handler);
    }

    bool MessageRouter::registerMessageHandler(QString command, messageHandler handler)
    {
        bool registeredMessageHandlerSuccessfull = FALSE;

        if(!registeredMessageHandlers->contains(command))
        {
            registeredMessageHandlers->insert(command, handler);
            registeredMessageHandlerSuccessfull = TRUE;
        }

        return registeredMessageHandlerSuccessfull;
    }

    bool MessageRouter::unregisterMessageHandler(QString command)
    {
        bool unregisteredMessageHandlerSuccessfull = FALSE;

        if(registeredMessageHandlers->contains(command))
        {
            registeredMessageHandlers->remove(command);
            unregisteredMessageHandlerSuccessfull = TRUE;
        }

        return unregisteredMessageHandlerSuccessfull;
    }

    void MessageRouter::onMessageParsed(Message* message)
    {
        QString command = message->getCommand();
        Message * responseMessage;

        if(registeredMessageHandlers->contains(command))
        {
            messageHandler handlerFunction = registeredMessageHandlers->value(command);
            responseMessage = ((handlerFunction.object)->*(handlerFunction.function))(command, message);
            WRITE_DEBUG("MessageRouter: Handled received message.")
        }
        else
        {
            responseMessage = new Message(QString("RESPONSE"), message->getReceiver(), message->getSender());
            responseMessage->addParameter(QString("status"), QString("unknown command"));
            WRITE_DEBUG("MessageRouter: Received unknown message.")
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

        emit writeMessage(responseMessage);
    }

} /* namespace ServerAppl */
