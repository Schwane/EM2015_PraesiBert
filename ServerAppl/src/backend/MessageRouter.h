/*
 * MessageRouter.h
 *
 *  Created on: 16.06.2015
 *      Author: sebastian
 */

#ifndef MESSAGEROUTER_H_
#define MESSAGEROUTER_H_

#include <QMap>
#include <QObject>
#include <QString>

#include <Message.hpp>

#include <src/backend/MessageHandlerInterface.h>

typedef Message* (ServerAppl::MessageHandlerInterface::*handleReceivedMessageFunction)(QString commandName, Message * message);
typedef struct
{
    ServerAppl::MessageHandlerInterface * object;
    handleReceivedMessageFunction function;
}messageHandler;

namespace ServerAppl
{
    /**
     * @class MessageRouter MessageRouter.h "src/backend/MessageRouter.h"
     *
     * @brief This class receives Message-objects and delivers them to target-objects.
     *
     * Message-objects can be given to this class over the slot onMessageParsed. The MessageRouter will inspect the clientId of the
     * transmitting client and will check in a table (registeredMessageHandlers) if handler-function for that Message-object exists.
     * If a handler-function exist it will be called. If not the MessageRouter will try to find a direct route in the table
     * directRoutingTable with the receiver-field of the Message-object. If no direkt-route was found this message-object will be deleted.
     *
     * An object/class that want to receive Message-objects from this MessageRouter needs to emplement the class MessageHandlerInterface.
     * If it does it can define handler-functions according to the type handleReceivedMessageFunction. These handler-functions can be added
     * to the table registeredMessageHandlers with the function registerMessageHandler of the MessageRouter-class.
     *
     * The signal writeMessage will be used to transmit response-messages.
     */
    class MessageRouter : public QObject
    {
        Q_OBJECT

    public:
        MessageRouter();
        ~MessageRouter();

        /**
         * @brief   Function to register a handler-function for a defined command and transmitter-client-id.
         *
         * @param[in]   clientId    ClientId of the client which transmitted the command to the server.
         * @param[in]   command     Name of the command which shall be handled.
         * @param[in]   object      Pointer to the object which contains the handler-function.
         * @param[in]   function    Pointer to the member-function which shall handle the command.
         *
         * @return  Returns true if the handler-function was registered successfully.
         *
         * The function will try to add the given handler-function to the registeredMessageHandler-table. If already an entry for
         * this command/client-combination exist a boolean false-value will be returned and no handler-function will be added.
         *
         */
        bool registerMessageHandler(uint clientId, QString command, MessageHandlerInterface * object, handleReceivedMessageFunction function);

        /**
         * @brief   Function to register a handler-function for a defined command and transmitter-client-id.
         *
         * @param[in]   clientId    ClientId of the client which transmitted the command to the server.
         * @param[in]   command     Name of the command which shall be handled.
         * @param[in]   handler     A structure with a pointer to the object with the handler-function and a function-pointer to the handler-function itself.
         *
         * @return  Returns true if the handler-function was registered successfully.
         *
         * The function will try to add the given handler-function to the registeredMessageHandler-table. If already an entry for
         * this command/client-combination exist a boolean false-value will be returned and no handler-function will be added.
         *
         */
        bool registerMessageHandler(uint clientId, QString command, messageHandler handler);

        /**
         * @brief   Function to remove a message-handler from the registeredMessageHandlers-table.
         *
         * @param[in]   clientId    ID of the client to which the message-handler belongs.
         * @param[in]   command     Command-name whose message-handler shall be removed.
         *
         * @return  Returns true if message-handler was found in the registeredMessageHandlers-table and successfully removed.
         */
        bool unregisterMessageHandler(uint clientId, QString command);

        /**
         * @brief   Function to remove all message-handler for commands from a client.
         *
         * @param[in]   clientId    All message-handlers for this client will be removed.
         */
        bool unregisterMessageHandlers(uint clientId);

        /**
         * @brief   Function to register a direct route.
         *
         * @param[in]   receiver    This is a string with the receiver of a message.
         * @param[in]   receiverId  ClientId of the receiver.
         *
         * @return  Returns true if the direct route was registered successfully.
         *
         * If for a received Message-object no message-handler could be found the MessageRouter will try to find the receiver-string
         * of the Message-object in the table directRoutingTable. If the directRouting table contains the receiver-string the
         * Message-object will be forwarded to the according clientId which is specified in the directRoutingTable.
         */
        bool addDirectRoute(QString receiver, uint receiverId);

        /**
         * @brief   Function to remove a direct route.
         *
         * @param[in]   receiver    A direct-route for this receiver will be removed.
         *
         * @return  Returns true if the route was found and successfully removed.
         */
        bool removeDirectRoute(QString receiver);

    signals:
        /**
         * @brief   This signal will be used to transmit response-messages for a received Message-object.
         *
         * @param[out]  message     Pointer to the message-object which shall be transmitted.
         * @param[out]  clientId    ID of the client to which the Message-object shall be transmitted.
         */
        void writeMessage(Message * message, uint clientId);

    public slots:
        /**
         * @brief   This slot handles received Message-objects
         *
         * @param[in]   message     Pointer to the Message-object which was received (will be deleted whithin the function!)
         * @param[in]   cliendId    ID of the client from which the Message-object was transmitted.
         *
         * This slot will try to find a message-handler (basing on the clientId and the command-name of the Message-object).
         * If no message-handler was found it will try to forward it on a direct-route (basing on the receiver-string
         * of the Message-object and the directRoutingTable) to a specific client.
         * If both failed (finding a message-handler and a direct-route) the Message-object will be deleted.
         */
        void onMessageParsed(Message * message, uint clientId);

    private:
        QMap<uint, QMap<QString, messageHandler> *> * registeredMessageHandlers;
        QMap<QString, uint> directRoutingTable;
    };
}



#endif /* MESSAGEROUTER_H_ */
