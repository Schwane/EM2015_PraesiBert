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
#include <Praesentation.hpp>
#include <ServerSocket.h>
#include <XMLMessageParser.hpp>
#include <XMLMessageWriter.hpp>

#include <src/backend/ByteStreamVerifier.h>
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
        bool registerMaster(Master * master);
        bool unregisterMaster(Master * master);
        bool registerListener(Listener * listener);
        QString getIpAddress();
        QString getCommandPort();
        QString getDataPort();

        /* Message handlers */
        Message* handleUnknownMessage(QString commandName, Message* msg);

    signals:
        void sendCmdToID(Message* msg, uint clientID);
        void sendCmdMessageToMultClients(Message* data, QList<uint> clientIDs);
        void sendDataMessageToMultClients(Message* data, QList<uint> clientIDs);
        void sendCmdMessageToAll(Message* msg);
        void gotIpAddress(QString ipAddress);

    public slots:
        void onStopPresentation();
        void onForwardMessageToClient(Message * msg, unsigned int clientId);
        void onForwaredMessageToMaster(Message * msg, unsigned int clientId);
        void onReceivedSetSlide(int slideNumber);
        void onNewClient(uint clientId);
        void onMasterAuthenticationFailed();
        void onMasterAuthentificationSuccessfull();
        void onReceivedPresentation(Praesentation * presentation, QMap<QString, QVariant> presentationParameterList, QMap<QString, QString> presentationParameterTypeList);
        void onDeliverPresentationToClient(unsigned int clientId);
        void onClientDisconnected(unsigned int clientId);
        void onNewIP(QString newIP);
        void onWriteAudioRecording(QString fileName, const QByteArray & recording);

    private:
        void deleteCommandRouter();
        void deleteDataRouter();
        void initCommandRouter();
        void initDataRouter();
        void transmitStopCommand(bool transmitToMaster);
        void transmitPresentationToClients();

    /* properties */
    public:
        static const char serverCommandPort[];
        static const char serverDataPort[];

    private:
        MessageRouter * commandRouter;
        MessageRouter * dataRouter;
        ByteStreamVerifier * byteStreamVerifier;
        XMLMessageParser * messageParser;
        XMLMessageWriter * messageWriter;
        Network::ServerSocket * serverSocket;
        PresentationController * presentationController;
        Master * masterClient;
        QMap <unsigned int, UnspecifiedClient *> connectedClients;
        QMap <unsigned int, Listener *> listenerClients;
        Praesentation * presentation;
        QMap<QString, QVariant> presentationParameterList;
        QMap<QString, QString> presentationParameterTypeList;
        int currentSlide;
        QString ipAddress;
        QString dataPort;
        QString commandPort;
    };

} /* namespace ServerAppl */

#endif /* SERVER_H_ */
