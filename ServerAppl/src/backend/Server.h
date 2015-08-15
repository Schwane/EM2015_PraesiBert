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
    /**
     * @class Server Server.h "src/backend/Server.h"
     *
     * @brief An object of this class is the central component of the server-application.
     *
     * The Server-class contains all other components of the server-application (client-lists, ByteStreamVerifier, ServerSocket,
     * MessageRouter, XmlMessageParser, XmlMessageWriter, ...).
     *
     * On initialization all objects will be created and the network-module (ServerSocket) will be configured.
     *
     * If the ServerSocket receives a new connection the slot onNewClient will be called. It will generate an UnspecifiedClient-object
     * and register all necessary message-handlers at the MessageRouter's (command- and data-router). Also the UnspecifiedClient-object
     * will be inserted into the list connectedClients.
     * When the UnspecifiedClient received a login-command (which shows the type of the client) the UnspecifiedClient-object will create
     * a Master- or Listener-object and give it to the Server-object (registerMaster- or registerListener-function). Here it will be
     * checked (e.g. for a Master-object if already exists one) and if everything is OK the object will be inserted into the
     * listenerClients-list or the masterClient-property. Afterwards the old UnspecifiedClient-object from the connectedClients-list
     * will be replaced with the new one (Master- or Listener-object).
     *
     * If the authentication-process of a master-object fails the slot onMasterAuthenticationFailed will delete all objects of the
     * master-client and disconnect the network-connection.
     *
     * If the master-object received a presentation the slot onReceivedPresentation will save the presentation in the Server-object
     * an afterwards it will forward the presentation to all listenerClients which are successfully registered at the server.
     * If further listener-Clients will connect the server the presentation will be transmitted to them right after successfull
     * login.
     *
     * If a Master- or Listener-object received an audio-recording the slot onWriteAudioRecording will write the audio-data into
     * the file-system of the device. The filename will be given as a parameter of the slot. The path will be the same as the
     * presentation used to save the slides.
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
