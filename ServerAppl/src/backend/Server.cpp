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
        presentationController = new PresentationController(this);
        masterClient = NULL;
        presentation = NULL;
        currentSlide = 0;

        QObject::connect(
                serverSocket, SIGNAL(newIP(QString)),
                this, SLOT(onNewIP(QString))
                );
        QObject::connect(
                serverSocket, SIGNAL(newIP(QString)),
                this, SLOT(gotIpAddress(QString))
                );

        if(serverSocket->beginListening(QString(serverCommandPort),QString(serverDataPort)))
        {
            this->commandPort = QString(serverCommandPort);
            this->dataPort = QString(serverDataPort);

            WRITE_DEBUG("Server-socket is listening.")

            QObject::connect(
                    serverSocket, SIGNAL(newClient(uint)),
                    this, SLOT(onNewClient(uint))
                    );

            QObject::connect(
                    serverSocket, SIGNAL(clientDisconnect(uint)),
                    this, SLOT(onClientDisconnected(unsigned int))
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
                    this, SIGNAL(sendCmdToID(Message*, uint)),
                    messageWriter, SLOT(writeCmdMessage(Message*, uint))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(cmdMessageWritten(QByteArray, uint)),
                    serverSocket, SLOT(sendCmdToID(QByteArray , uint ))
                    );

//            QObject::connect(
//                    this, SIGNAL(sendCmdToID(Message*, uint)),
//                    messageWriter, SLOT(writeCmdMessage(Message*, uint))
//                    );

            QObject::connect(
                    this, SIGNAL(sendCmdMessageToAll(Message*)),
                    messageWriter, SLOT(writeCmdMessage(Message*))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(cmdMessageWritten(QByteArray)),
                    serverSocket, SLOT(sendCmdToAll(QByteArray))
                    );

            QObject::connect(
                    this, SIGNAL(sendCmdMessageToMultClients(Message*, QList<uint>)),
                    messageWriter, SLOT(writeCmdMessage(Message*, QList<uint>))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(cmdMessageWritten(QByteArray, QList<uint>)),
                    serverSocket, SLOT(sendCmdToMultClients(QByteArray, QList<uint>))
                    );

            /* connect signals to data-router */
            QObject::connect(
                    serverSocket, SIGNAL(receivedDataFromClient(QByteArray , uint)),
                    byteStreamVerifier, SLOT(verifyDataByteStream(QByteArray, uint))
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
                    this, SIGNAL(sendDataMessageToMultClients(Message*, QList<uint>)),
                    messageWriter, SLOT(writeDataMessage(Message*, QList<uint>))
                    );

            QObject::connect(
                    messageWriter, SIGNAL(dataMessageWritten(QByteArray, QList<uint>)),
                    serverSocket, SLOT(sendDataToMultClients(QByteArray, QList<uint>))
                    );

        }

        WRITE_DEBUG("Server-constructor finished.")
    }

    Server::~Server()
    {
        if(presentation)
        {
            delete(presentation);
        }

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

    Message* Server::handleUnknownMessage(QString commandName, Message* msg)
    {
        return NULL;
    }

    void Server::onForwardMessageToClient(Message* msg, unsigned int clientId)
    {
        if(connectedClients.contains(clientId))
        {
            emit sendCmdToID(msg, clientId);
        }
    }

    void Server::onForwaredMessageToMaster(Message* msg, unsigned int clientId)
    {
        if(connectedClients.contains(clientId)
                && NULL != masterClient
                )
        {
            emit sendCmdToID(msg, masterClient->getClientId());
        }
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
            WRITE_DEBUG(clientId)
        }
        else
        {
            //TODO: handle double-occurence of clientId!
            WRITE_DEBUG("New client tried to connect with existing clientId.")
        }
    }

    void Server::onMasterAuthenticationFailed()
    {
        WRITE_DEBUG("onMasterAuthenticationFailed called.")
//        commandRouter->unregisterMessageHandlers(masterClient->getClientId());
        connectedClients.remove(masterClient->getClientId());
        this->unregisterMaster(masterClient);
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
                DATA_PRAESENTATION,
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

    void Server::onReceivedSetSlide(int slideNumber)
    {
        Message * setSlideMessage = NULL;
        int listenerCounter = listenerClients.count() - 1;
        QList<Listener* > allListeners = listenerClients.values();
        QList<uint> clientIds;

        while(0 <= listenerCounter)
        {
            if(allListeners.value(listenerCounter)->getHasPresentation())
            {
                clientIds.append(allListeners.value(listenerCounter)->getClientId());
            }
            listenerCounter--;
        }

        if(masterClient)
        {
            clientIds.append(masterClient->getClientId());
        }

        this->currentSlide = slideNumber;

        setSlideMessage = new Message(CMD_SET_SLIDE, "server", "client");
        setSlideMessage->addParameter("slide", slideNumber);

        emit sendCmdMessageToMultClients(setSlideMessage, clientIds);
    }

    void Server::onClientDisconnected(unsigned int clientId)
    {
        if(connectedClients.contains(clientId))
        {
            commandRouter->unregisterMessageHandlers(clientId);
            dataRouter->unregisterMessageHandlers(clientId);

            if(listenerClients.contains(clientId))
            {
                Listener * disconnectedClient = listenerClients.value(clientId);
                connectedClients.remove(clientId);
                listenerClients.remove(clientId);

                QObject::disconnect(
                        disconnectedClient, SIGNAL(forwaredMessageToMaster(Message *, unsigned int)),
                    this, SLOT(onForwaredMessageToMaster(Message *, unsigned int))
                    );

                QObject::disconnect(
                        disconnectedClient, SIGNAL(requestDeliverPresentation( unsigned int)),
                    this, SLOT(onDeliverPresentationToClient(unsigned int))
                    );

                commandRouter->removeDirectRoute(QString(clientId));

                delete(disconnectedClient);

                WRITE_DEBUG("ListenerClient disconnected.")
            }
            else if(NULL != masterClient
                    && masterClient->getClientId() == clientId
                    )

            {
//                this->byteStreamVerifier->removeMessageAuthenticator(clientId);
//                connectedClients.remove(clientId);
                this->transmitStopCommand(false);

                if(this->presentation)
                {
                    delete(this->presentation);
                    this->presentation = NULL;
                }

                unregisterMaster(masterClient);
                delete(masterClient);
                masterClient = NULL;

                WRITE_DEBUG("MasterClient disconnected.")
            }
            else
            {
                UnspecifiedClient * disconnectedClient = connectedClients.value(clientId);
                connectedClients.remove(clientId);
                delete(disconnectedClient);

                WRITE_DEBUG("UnspecifiedClient disconnected.")
            }
        }
    }

    void Server::onStopPresentation()
    {
        this->transmitStopCommand(true);

    }

    void Server::onDeliverPresentationToClient(unsigned int clientId)
    {
        if(this->listenerClients.contains(clientId)
                && NULL != presentation
                )
        {
            Message * presentationMessage = new Message(CMD_SET_PRAESENTATION, "server", "client");
            Message * setSlideMessage = new Message(CMD_SET_SLIDE, "server", "client");

            presentationMessage = this->presentation->packPraesentation(presentationMessage);
            setSlideMessage->addParameter("slide", this->currentSlide);

            emit sendCmdToID(presentationMessage, clientId);
            emit sendCmdToID(setSlideMessage, clientId);

            this->listenerClients.value(clientId)->setHasPresentation(true);
        }
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

    QString Server::getIpAddress()
    {
        return this->ipAddress;
    }

    QString Server::getCommandPort()
    {
        return this->commandPort;
    }

    QString Server::getDataPort()
    {
        return this->dataPort;
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
                QObject::connect(
                        master, SIGNAL(receivedPresentation(Praesentation *, QMap<QString, QVariant>, QMap<QString, QString>)),
                        this, SLOT(onReceivedPresentation(Praesentation *, QMap<QString, QVariant>, QMap<QString, QString>))
                        );
                QObject::connect(
                        master, SIGNAL(receivedSetSlide(int)),
                        this, SLOT(onReceivedSetSlide(int))
                        );

                QObject::connect(
                        master, SIGNAL(forwardMessageToClient(Message *, unsigned int)),
                        this, SLOT(onForwardMessageToClient(Message*, unsigned int))
                        );

                QObject::connect(
                        master, SIGNAL(stopPresentation()),
                        this, SLOT(onStopPresentation())
                        );



                commandRouter->unregisterMessageHandler(clientId, CMD_LOGIN);
                commandRouter->unregisterMessageHandler(clientId, CMD_AUTH_PHASE1);
                commandRouter->registerMessageHandler(
                        clientId,
                        CMD_AUTH_PHASE3,
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(Master::handleAuthenticationPhase3)
                        );
//                commandRouter->registerMessageHandler(
//                        clientId,
//                        CMD_ACK_RESPONSE,
//                        HANDLER_OBJ(master),
//                        HANDLER_FUNC(Master::handleAuthenticationAcknowledge)
//                        );
                commandRouter->registerMessageHandler(
                        clientId,
                        CMD_SET_SLIDE,
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(Master::handleSetSlide)
                        );
                commandRouter->registerMessageHandler(
                        clientId,
                        CMD_STOP_PRAESENTATION,
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(Master::handleStopPresentation)
                        );
                commandRouter->addDirectRoute(QString("master") , master->getClientId());

                dataRouter->registerMessageHandler(
                        clientId,
                        DATA_PRAESENTATION,
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(Master::handleDataPresentation)
                        );

                this->byteStreamVerifier->addMessageAuthenticator(master->getClientId(), master->getMessageAuthenticator());

                WRITE_DEBUG("Registered master client successfully.")
                registrationSuccessfull = TRUE;
            }
        }

        return registrationSuccessfull;
    }

    bool Server::unregisterMaster(Master * master)
    {
        QObject::disconnect(
                master, SIGNAL(authenticationFailed()),
                this, SLOT(onMasterAuthenticationFailed())
                );
        QObject::disconnect(
                master, SIGNAL(authenticationSuccessfull()),
                this, SLOT(onMasterAuthentificationSuccessfull())
                );
        QObject::disconnect(
                master, SIGNAL(receivedPresentation(Praesentation *, QMap<QString, QVariant>, QMap<QString, QString>)),
                this, SLOT(onReceivedPresentation(Praesentation *, QMap<QString, QVariant>, QMap<QString, QString>))
                );
        QObject::disconnect(
                master, SIGNAL(receivedSetSlide(int)),
                this, SLOT(onReceivedSetSlide(int))
                );

        QObject::connect(
                master, SIGNAL(forwardMessageToClient(Message *, unsigned int)),
                this, SLOT(onForwardMessageToClient(Message*, unsigned int))
                );

        QObject::connect(
                master, SIGNAL(stopPresentation()),
                this, SLOT(onStopPresentation())
                );

        this->byteStreamVerifier->removeMessageAuthenticator(master->getClientId());

        commandRouter->removeDirectRoute(QString(master->getClientId()));
        commandRouter->unregisterMessageHandlers(master->getClientId());
        dataRouter->unregisterMessageHandlers(master->getClientId());

        return true;
    }

    bool Server::registerListener(Listener* listener)
    {
        bool registrationSuccessfull = FALSE;

        if(listener)
        {
            uint clientId = listener->getClientId();
            WRITE_DEBUG("registerListener: listener NOT null")

            if(connectedClients.contains(clientId)
                    && !listenerClients.contains(clientId)
                    )
            {
                QString listenerId;

                listenerId.setNum(clientId);

                listenerClients.insert(clientId, listener);
                connectedClients.insert(clientId, (UnspecifiedClient*) listener);

                QObject::connect(
                    listener, SIGNAL(forwaredMessageToMaster(Message *, unsigned int)),
                    this, SLOT(onForwaredMessageToMaster(Message *, unsigned int))
                    );

                QObject::connect(
                    listener, SIGNAL(requestDeliverPresentation( unsigned int)),
                    this, SLOT(onDeliverPresentationToClient(unsigned int))
                    );

                commandRouter->unregisterMessageHandler(clientId, CMD_AUTH_PHASE1);
                commandRouter->unregisterMessageHandler(clientId, CMD_LOGIN);

                commandRouter->registerMessageHandler(
                        clientId,
                        QString(CMD_ACK_RESPONSE),
                        HANDLER_OBJ(listener),
                        HANDLER_FUNC(Listener::handleAcknowledge)
                        );

                commandRouter->addDirectRoute(listenerId , listener->getClientId());

                registrationSuccessfull = TRUE;
            }
        }
        WRITE_DEBUG("leaving registerListener.")
        return registrationSuccessfull;
    }

    void Server::onNewIP(QString newIP)
    {
        this->ipAddress = newIP;
    }

    void Server::transmitPresentationToClients()
    {
        Message * presentationMessage = new Message(CMD_SET_PRAESENTATION, "server", "client");
        QList<Listener* > allListeners = listenerClients.values();
        int listenerCounter = allListeners.length() - 1;
        QList<uint> clientIds;

//        this->presentation = presentation;

//        presentationMessage->setParameterList(this->presentationParameterList);
//        presentationMessage->setParameterTypeList(this->presentationParameterTypeList);

        presentationMessage = this->presentation->packPraesentation(presentationMessage);

        while(0 <= listenerCounter)
        {
//            if(!allListeners.value(listenerCounter)->getHasPresentation())
//            {
                clientIds.append(allListeners.value(listenerCounter)->getClientId());
                allListeners.value(listenerCounter)->setHasPresentation(true);
//            }

            listenerCounter--;
        }

//        if(masterClient)
//        {
//
//        }

        emit sendDataMessageToMultClients(presentationMessage, clientIds);
//        emit sendDataMessageToMultClients(presentationMessage, *(this->getAllClientIdentifiers()));

    }

    void Server::transmitStopCommand(bool transmitToMaster)
    {
        Message * stopMessage = new Message(CMD_STOP_PRAESENTATION, "server", "clients");

        if(transmitToMaster)
        {
            emit sendCmdMessageToAll(stopMessage);
        }
        else
        {
            QList<unsigned int> * listenerClients = this->getListenerClientIdentifiers();

            emit sendCmdMessageToMultClients(stopMessage, *listenerClients);

            delete(listenerClients);
        }
    }

    void Server::initDataRouter()
    {
    }

} /* namespace ServerAppl */
