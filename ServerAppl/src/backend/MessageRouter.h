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
    class MessageRouter : public QObject
    {
        Q_OBJECT

    public:
        MessageRouter();
        ~MessageRouter();
        bool registerMessageHandler(uint clientId, QString command, MessageHandlerInterface * object, handleReceivedMessageFunction function);
        bool registerMessageHandler(uint clientId, QString command, messageHandler handler);
        bool unregisterMessageHandler(uint clientId, QString command);
        bool unregisterMessageHandlers(uint clientId);

    signals:
        void writeMessage(Message * message, uint clientId);

    public slots:
        void onMessageParsed(Message * message, uint clientId);

    private:
        QMap<uint, QMap<QString, messageHandler> *> * registeredMessageHandlers;
    };
}



#endif /* MESSAGEROUTER_H_ */
