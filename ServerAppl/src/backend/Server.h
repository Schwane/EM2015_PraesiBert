/*
 * Server.h
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <QHostAddress>
#include <QList>
#include <QMap>
#include <QObject>

#include <Message.hpp>
#include <ServerSocket.h>
#include <XMLMessageParser.hpp>
#include <XMLMessageWriter.hpp>

#include <src/backend/Listener.h>
#include <src/backend/Master.h>
#include <src/backend/MessageHandlerInterface.h>
#include <src/backend/MessageRouter.h>

namespace ServerAppl
{
    /* Forward-Declaration */
    class PresentationController;

    /**
     *
     */
    class Server : public QObject, public MessageHandlerInterface
    {
        Q_OBJECT

    /* functions */
    public:
        Server();
        virtual ~Server();
        QList<unsigned int> * getAllClientIdentifiers();
        QList<unsigned int> * getListenerClientIdentifiers();
        unsigned int getMasterClientIdentifier();
        Message* handleReceivedMessage(QString commandName, Message* msg);

    public slots:
        void onNewClient();

    private:
        void deleteCommandRouter();
        void deleteDataRouter();
        void initCommandRouter();
        void initDataRouter();

    /* properties */
    public:
        XMLMessageParser * messageParser;

    private:
        MessageRouter * commandRouter;
        MessageRouter * dataRouter;
        XMLMessageWriter * messageWriter;
        Network::ServerSocket * serverSocket;
        PresentationController * presentationController;
        Master * masterClient;
        QMap <unsigned int, Listener *> listenerClients;


    };

} /* namespace ServerAppl */

#endif /* SERVER_H_ */
