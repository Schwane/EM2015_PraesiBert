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
        WRITE_DEBUG("registerMessageHandler called")

        bool registeredMessageHandlerSuccessfull = FALSE;
        QMap<QString, messageHandler> * clientCommands;

        if(!registeredMessageHandlers->contains(clientId))
        {
            messageHandler unkownCommandHandler;

            unkownCommandHandler.object = handler.object;
            unkownCommandHandler.function = HANDLER_FUNC(MessageHandlerInterface::handleUnknownMessage);

            clientCommands = new QMap<QString, messageHandler>();
            clientCommands->insert("unknown_command", unkownCommandHandler);

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

    bool MessageRouter::unregisterMessageHandlers(uint clientId)
    {
        bool unregisteredMessageHandlerSuccessfull = FALSE;

        if(registeredMessageHandlers->contains(clientId))
        {
            delete(registeredMessageHandlers->value(clientId));
            registeredMessageHandlers->remove(clientId);
        }

        return unregisteredMessageHandlerSuccessfull;
    }

    bool MessageRouter::addDirectRoute(QString receiver, uint receiverId)
    {
        bool addedSuccessfully = FALSE;

        if(!directRoutingTable.contains(receiver))
        {
            directRoutingTable.insert(receiver, receiverId);
            addedSuccessfully = TRUE;
        }

        return addedSuccessfully;
    }

    bool MessageRouter::removeDirectRoute(QString receiver)
    {
        bool removedSuccessfully = FALSE;

        if(directRoutingTable.contains(receiver))
        {
            directRoutingTable.remove(receiver);
            removedSuccessfully = TRUE;
        }

        return removedSuccessfully;
    }

    void MessageRouter::onMessageParsed(Message* message, uint clientId)
    {
        QString command = message->getCommand();
        QMap<QString, messageHandler> * clientCommands;
        Message * responseMessage = NULL;

        WRITE_DEBUG("onMessageParsed: received Message")
        WRITE_DEBUG(message->getCommand())

        if(registeredMessageHandlers->contains(clientId))
        {
            clientCommands = registeredMessageHandlers->value(clientId);
            if(clientCommands->contains(command))
            {
                messageHandler handlerFunction = clientCommands->value(command);
                responseMessage = ((handlerFunction.object)->*(handlerFunction.function))(command, message);
            }
            else
            {
                if(directRoutingTable.contains(message->getReceiver()))
                {
                    uint receiverId = directRoutingTable.value(message->getReceiver());
                    emit writeMessage(message, receiverId);
                }
                else
                {
                    WRITE_DEBUG("MessageRouter: Received unknown message.")
                    delete(message);
                }
            }
        }
        else
        {
            WRITE_DEBUG("No client found with this id.")
            delete(message);
            //TODO: client not registered. Maybe thrown an exception or emit error-signal?
        }

        if(!responseMessage)
        {
            WRITE_DEBUG("response message is null")
        }
        else
        {
            WRITE_DEBUG("emit write message signal..")
            WRITE_DEBUG(responseMessage->getCommand());
            emit writeMessage(responseMessage, clientId);
        }
    }

} /* namespace ServerAppl */
