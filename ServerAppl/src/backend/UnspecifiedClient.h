/*
 * Client.h
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <QObject>
#include <QString>
#include <QTime>

#include <Message.hpp>

#include <src/backend/MessageHandlerInterface.h>

namespace ServerAppl
{
    /* Forward-Declaration */
    class Server;

    class UnspecifiedClient : public QObject, public MessageHandlerInterface
    {
        Q_OBJECT

    public:
        UnspecifiedClient();
        UnspecifiedClient(Server * server, uint clientId, QString name);
        virtual ~UnspecifiedClient();
        unsigned int getClientId();
        Message* handleReceivedMessage(QString commandName, Message* msg);
        Message* handleLoginMessages(QString commandName, Message* msg);

        QString getName();
        QTime getLastTimestamp();
        Server * getServer();

    protected:
        Server * server;
        uint clientId;
        QString name;
        QTime lastTimestamp;
    };

} /* namespace ServerAppl */

#endif /* CLIENT_H_ */
