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

enum ClientType
{
    ClientType_Unspecified,
    ClientType_Listener,
    ClientType_Master
};

namespace ServerAppl
{
    /* Forward-Declaration */
    class Server;

    /**
     * @class UnspecifiedClient UnspecifiedClient.h "src/backend/UnspecifiedClient.h"
     *
     * @brief Objects of this class represent a client the has not shown if its a master-client or listener-client.
     *
     * The class UnspecifiedClient will represent a client for the time after the network-connection was established but no login-procedure
     * was performed. In this time it is not clear what kind of client it is.
     * After receiving a login-command it can be defined if its a master-client or a listener-client. It then will be transformed into an
     * object of the classes Master or Listener. Both classes offer static functions which will build an Master/Listener-object basing on
     * the data of a UnspecifiedClient-object.
     *
     * Further this class is the base-class of Mater/Client-class. It defines a common interface for both classes and some basic-functionallity.
     *
     */
    class UnspecifiedClient : public QObject, public MessageHandlerInterface
    {
        Q_OBJECT

    public:
        UnspecifiedClient();
        UnspecifiedClient(Server * server, uint clientId, QString name);
        virtual ~UnspecifiedClient();

        /**
         * @brief   Returns the unique ID of the client.
         *
         * @return  Unsigned integer which is the ID of the client.
         *
         * The ID's of the clients will be generated by the network-module.
         */
        unsigned int getClientId();

        /**
         *  @brief  This function returns the type of the client.
         *
         *  @return Will return ClientType_Unspecified, ClientType_Listener or ClientType_Master.
         */
        virtual ClientType getClientType();

        /* Message handlers */
        Message* handleUnknownMessage(QString commandName, Message* msg);

        /**
         * @brief   Message-handler for login-commands of the client.
         *
         * If the command transmitted a login-command to the server this function shall handle the command. Only listener-clients will
         * transmit this command. After receiving this command the UnspecifiedClient-object will be transformed into a Listener-object.
         * The new object will be verified by the Server-object.
         */
        Message* handleLoginMessages(QString commandName, Message* msg);

        /**
         * @brief   Message-handler for auth-phase-1-commands
         *
         * Only a master-client will transmit this command. It is the first command in the login-phase of the master-client. After
         * receiving this command the UnspecifiedClient-object will be transformed into a Master-object and the Server-object will
         * verify the new Master-object.
         */
        Message* handleAuthPhase1(QString commandName, Message * msg);

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
