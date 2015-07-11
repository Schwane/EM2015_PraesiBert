/*
 * Server.cpp
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#include <src/backend/Server.h>

#include <commands.hpp>

#include <src/backend/Listener.h>
#include <src/backend/Master.h>
#include <src/backend/PresentationController.h>
#include <src/backend/Logger.h>

namespace ServerAppl
{
    const char Server::serverCommandPort[] = "1337";
    const char Server::serverDataPort[] = "1338";

    Server::Server()
    {
        WRITE_DEBUG("Entered server-construtor.")
        serverSocket = new Network::ServerSocket(this);
        byteStreamVerifier = new ByteStreamVerifier();
        messageParser = new XMLMessageParser();
        messageWriter = new XMLMessageWriter();
        commandRouter =  new MessageRouter();
        dataRouter = new MessageRouter();
        connectedClients = QMap <unsigned int, UnspecifiedClient *>();
        listenerClients = QMap <unsigned int, Listener *>();
        presentationController = new PresentationController(this);
        masterClient = NULL;

        if(serverSocket->beginListening(QString(serverCommandPort),QString(serverDataPort)))
        {
            WRITE_DEBUG("Server-socket is listening.")
            QObject::connect(
                        serverSocket, SIGNAL(newClient(uint)),
                        this, SLOT(onNewClient(uint))
                        );
            /* connect signals to command-router */
            QObject::connect(
                    serverSocket, SIGNAL(receivedCmdFromClient(QByteArray , uint)),
                    byteStreamVerifier, SLOT(verifyCmdByteStream(QByteArray, uint))
                    );

            QObject::connect(
                    byteStreamVerifier, SIGNAL(cmdByteStreamVerified(QByteArray, uint)),
                    messageParser, SLOT(parseCmdMessage(QByteArray, uint))
                    );

            QObject::connect(
                    messageParser, SIGNAL(cmdMessageParsed(Message*, uint)),
                    commandRouter, SLOT(onMessageParsed(Message*, uint))
                    );
            QObject::connect(
                    commandRouter, SIGNAL(writeMessage(Message*, uint)),
                    messageWriter, SLOT(writeCmdMessage(Message*, uint))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(cmdMessageWritten(QByteArray, uint)),
                    serverSocket, SLOT(sendCmdToID(QByteArray , uint ))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(cmdMessageWritten(QByteArray, QList<uint>)),
                    serverSocket, SLOT(sendCmdToMultClients(QByteArray, QList<uint>))
                    );

            /* connect signals to data-router */
            QObject::connect(
                    serverSocket, SIGNAL(receivedDataFromClient(QByteArray , uint)),
                    byteStreamVerifier, SLOT(verifyCmdByteStream(QByteArray, uint))
                    );

            QObject::connect(
                    byteStreamVerifier, SIGNAL(dataByteStreamVerified(QByteArray, uint)),
                    messageParser, SLOT(parseDataMessage(QByteArray, uint))
                    );

            QObject::connect(
                    messageParser, SIGNAL(dataMessageParsed(Message*, uint)),
                    dataRouter, SLOT(onMessageParsed(Message*, uint))
                    );
            QObject::connect(
                    dataRouter, SIGNAL(writeMessage(Message*, uint)),
                    messageWriter, SLOT(writeDataMessage(Message*, uint))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(dataMessageWritten(QByteArray, uint)),
                    serverSocket, SLOT(sendDataToID(QByteArray , uint ))
                    );

            QObject::connect(
                    this, SIGNAL(sendDataMessageToMultClients(QByteArray, QList<uint>)),
                    messageWriter, SLOT(writeDataMessage(Message*, QList<uint>))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(dataMessageWritten(QByteArray, QList<uint>)),
                    serverSocket, SLOT(dataCmdToMultClients(QByteArray, QList<uint>))
                    );

        }

        WRITE_DEBUG("Server-constructor finished.")
    }

    Server::~Server()
    {
        if(masterClient)
        {
            delete(masterClient);
        }


        delete(presentationController);
        delete(dataRouter);
        delete(commandRouter);
        delete(messageWriter);
        delete(messageParser);
        delete(byteStreamVerifier);
        delete(serverSocket);

        WRITE_DEBUG("Server-destructor finished.")
    }

    Message* Server::handleReceivedMessage(QString commandName, Message* msg)
    {
        return NULL;
    }

    void Server::onNewClient(uint clientId)
    {
        UnspecifiedClient * newClient;

        if(!connectedClients.contains(clientId))
        {
            newClient = new UnspecifiedClient(this, clientId, QString(""));
            connectedClients.insert(clientId, newClient);

            commandRouter->registerMessageHandler(
                    clientId,
                    QString(CMD_LOGIN),
                    HANDLER_OBJ(newClient),
                    HANDLER_FUNC(UnspecifiedClient::handleLoginMessages)
                    );

            commandRouter->registerMessageHandler(
                    clientId,
                    QString(CMD_AUTH_PHASE1),
                    HANDLER_OBJ(newClient),
                    HANDLER_FUNC(UnspecifiedClient::handleAuthPhase1)
                    );
            WRITE_DEBUG("Added new client to connected clients.")
        }
        else
        {
            //TODO: handle double-occurence of clientId!
            WRITE_DEBUG("New client tried to connect with existing clientId.")
        }
    }

    void Server::onMasterAuthenticationFailed()
    {
        commandRouter->unregisterMessageHandlers(masterClient->getClientId());
        connectedClients.remove(masterClient->getClientId());
        delete(masterClient);
        masterClient = NULL;
    }

    void Server::onMasterAuthentificationSuccessfull()
    {
        QObject::connect(
                masterClient, SIGNAL(receivedPresentation(Praesentation *, QMap<QString, QVariant>, QMap<QString, QString>)),
                this, SLOT(onReceivedPresentation(Praesentation *, QMap<QString, QVariant>, QMap<QString, QString>))
                );
        dataRouter->registerMessageHandler(
                masterClient->getClientId(),
                DATA_PRESENTAION,
                HANDLER_OBJ(masterClient),
                HANDLER_FUNC(Master::handleDataPresentation)
                );
    }

    void Server::onReceivedPresentation(Praesentation* presentation,
            QMap<QString, QVariant> presentationParameterList,
            QMap<QString, QString> presentationParameterTypeList)
    {
        this->presentation = presentation;
        this->presentationParameterList = presentationParameterList;
        this->presentationParameterTypeList = presentationParameterTypeList;

        this->transmitPresentationToClients();
    }

    QList<unsigned int>* Server::getAllClientIdentifiers()
    {
        QList<unsigned int> * identifierList;

        identifierList = getListenerClientIdentifiers();
        identifierList->append(getMasterClientIdentifier());

        return identifierList;
    }

    QList<unsigned int>* Server::getListenerClientIdentifiers()
    {
        QList<unsigned int>* listenerIdentifiers;

        listenerIdentifiers = new QList<unsigned int>(listenerClients.keys());

        return listenerIdentifiers;
    }

    unsigned int Server::getMasterClientIdentifier()
    {
        return masterClient->getClientId();
    }

    void Server::deleteCommandRouter()
    {
    }

    void Server::deleteDataRouter()
    {
    }

    void Server::initCommandRouter()
    {
    }

    bool Server::registerMaster(Master* master)
    {
        bool registrationSuccessfull = FALSE;

        if(master)
        {
            uint clientId = master->getClientId();

            if(!masterClient
                && connectedClients.contains(clientId)
                )
            {
                masterClient = master;

                QObject::connect(
                        master, SIGNAL(authenticationFailed()),
                        this, SLOT(onMasterAuthenticationFailed())
                        );
                QObject::connect(
                        master, SIGNAL(authenticationSuccessfull()),
                        this, SLOT(onMasterAuthentificationSuccessfull())
                        );

                commandRouter->unregisterMessageHandler(clientId, CMD_LOGIN);
                commandRouter->unregisterMessageHandler(clientId, CMD_AUTH_PHASE1);
                commandRouter->registerMessageHandler(
                        clientId,
                        CMD_AUTH_PHASE3,
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(Master::handleAuthenticationPhase3)
                        );
                commandRouter->registerMessageHandler(
                        clientId,
                        CMD_ACK_RESPONSE,
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(Master::handleAuthenticationAcknowledge)
                        );

                this->byteStreamVerifier->addMessageAuthenticator(master->getClientId(), master->getMessageAuthenticator());

                WRITE_DEBUG("Registered master client successfully.")
                registrationSuccessfull = TRUE;
            }
        }

        return registrationSuccessfull;
    }

    bool Server::registerListener(Listener* listener)
    {
        bool registrationSuccessfull = FALSE;
        WRITE_DEBUG("registerListener was called.")
        if(listener)
        {
            uint clientId = listener->getClientId();
            WRITE_DEBUG("registerListener: listener NOT null")

//            if(!connectedClients.contains(clientId))
//            WRITE_DEBUG("connectedClients NULL")

//            if(!listenerClients.contains(clientId))
//            WRITE_DEBUG("listenerClients:")
            listenerClients.isEmpty();
            WRITE_DEBUG("ddd")

            if(connectedClients.contains(clientId)
                    && !listenerClients.contains(clientId)
                    )
            {
                WRITE_DEBUG("aaa")
                listenerClients.insert(clientId, listener);
                connectedClients.insert(clientId, (UnspecifiedClient*) listener);

                commandRouter->unregisterMessageHandler(clientId, CMD_AUTH_PHASE1);
                commandRouter->unregisterMessageHandler(clientId, CMD_LOGIN);
//                commandRouter->registerMessageHandler(
//                        clientId,
//                        QString(CMD_LOGIN_ACK),
//                        HANDLER_OBJ(listener),
//                        HANDLER_FUNC(Listener::handleLoginAcknowledge)
//                        );
                registrationSuccessfull = TRUE;
            }
        }

        return registrationSuccessfull;
    }

    void Server::transmitPresentationToClients()
    {
        Message * presentationMessage = new Message(DATA_PRESENTAION, "server", "client");
        QList<Listener* > allListeners = listenerClients.values();
        int listenerCounter = allListeners.length() - 1;
        QList<uint> clientIds;

        this->presentation = presentation;

        presentationMessage->setParameterList(this->presentationParameterList);
        presentationMessage->setParameterTypeList(this->presentationParameterTypeList);

        while(0 <= listenerCounter)
        {
            if(!allListeners.value(listenerCounter)->getHasPresentation())
            {
                clientIds.append(allListeners.value(listenerCounter)->getClientId());
                allListeners.value(listenerCounter)->setHasPresentation(true);
            }
        }

        emit sendDataMessageToMultClients(presentationMessage, clientIds);

    }

    void Server::initDataRouter()
    {
    }

} /* namespace ServerAppl */
