/*
 * Server.cpp
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#include <src/backend/Server.h>

#include <src/backend/Listener.h>
#include <src/backend/Master.h>
#include <src/backend/PresentationController.h>
#include <src/backend/Logger.h>

namespace ServerAppl
{

    Server::Server()
    {
        WRITE_DEBUG("Entered server-construtor.")
        serverSocket = new Network::ServerSocket(this);
        messageParser = new XMLMessageParser();
        messageWriter = new XMLMessageWriter();
        commandRouter =  new MessageRouter();
        dataRouter = new MessageRouter();
        listenerClients = QMap <unsigned int, Listener *>();
        presentationController = new PresentationController(this);
        masterClient = NULL;

        /* connect signals of command-router */
        QObject::connect( messageParser, SIGNAL(cmdMessageParsed(Message*, uint)), commandRouter, SLOT(onMessageParsed(Message*, uint)) );
        QObject::connect( commandRouter, SIGNAL(writeMessage(Message*, uint)), messageWriter, SLOT(writeCmdMessage(Message*, uint)) );

        /* connect signals of command-router */
        QObject::connect( messageParser, SIGNAL(dataMessageParsed(Message*, uint)), dataRouter, SLOT(onMessageParsed(Message*, uint)) );
        QObject::connect( dataRouter, SIGNAL(writeMessage(Message*, uint)), messageWriter, SLOT(writeDataMessage(Message*, uint)) );

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
                    QString("login"),
                    HANDLER_OBJ(newClient),
                    HANDLER_FUNC(handleReceivedMessage)
                    );
            commandRouter->registerMessageHandler(
                    clientId,
                    QString("LOGIN_NONCE"),
                    HANDLER_OBJ(newClient),
                    HANDLER_FUNC(handleReceivedMessage)
                    );
        }
        else
        {
            //TODO: handle double-occurence of clientId!
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

                commandRouter->unregisterMessageHandler(clientId, QString("LOGIN_NONCE"));
                commandRouter->registerMessageHandler(
                        clientId,
                        QString("LOGIN_RESPONSE"),
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(handleReceivedMessage)
                        );
                commandRouter->registerMessageHandler(
                        clientId,
                        QString("PROOF_RESPONSE"),
                        HANDLER_OBJ(master),
                        HANDLER_FUNC(handleReceivedMessage)
                        );

                registrationSuccessfull = TRUE;
            }
        }

        return registrationSuccessfull;
    }

    bool Server::registerListener(Listener* listener)
    {
        bool registrationSuccessfull = FALSE;

        if(listener)
        {
            uint clientId = listener->getClientId();

            if(connectedClients.contains(clientId)
                    && !listenerClients.contains(clientId)
                    )
            {
                listenerClients.insert(clientId, listener);
                connectedClients.insert(clientId, (UnspecifiedClient*) listener);

                commandRouter->unregisterMessageHandler(clientId, QString("login"));
                commandRouter->registerMessageHandler(
                        clientId,
                        QString("login_RESPONSE"),
                        HANDLER_OBJ(listener),
                        HANDLER_FUNC(handleReceivedMessage)
                        );
                registrationSuccessfull = TRUE;
            }
        }

        return registrationSuccessfull;
    }

    void Server::initDataRouter()
    {
    }

} /* namespace ServerAppl */
