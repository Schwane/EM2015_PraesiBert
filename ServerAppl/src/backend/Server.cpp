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
        listenerClients = QMap <unsigned int, Listener *>();
        presentationController = new PresentationController(this);
        masterClient = NULL;

        registerMessageHandler(QString("login"), (MessageHandlerInterface *)this, &MessageHandlerInterface::handleReceivedMessage);

        QObject::connect(messageParser, SIGNAL(messageParsed(Message *)), this, SLOT(onMessageParsed(Message *)) );
        QObject::connect(this, SIGNAL(writeMessage(Message *)), messageWriter, SLOT(writeMessage(Message*)) );

        WRITE_DEBUG("Server-constructor finished.")
    }

    Server::~Server()
    {
        if(masterClient)
        {
            delete(masterClient);
        }


        delete(presentationController);
        delete(messageWriter);
        delete(messageParser);
//        delete(xmlWriter);
//        delete(xmlReader);
        delete(serverSocket);

        WRITE_DEBUG("Server-destructor finished.")
    }

    Message* Server::handleReceivedMessage(QString commandName, Message* msg)
    {

    }

    void Server::onNewClient()
    {
        if(FALSE)
        {
            /* create a listener-client */
            Listener * listenerClient;
            listenerClient = new Listener();
            listenerClients.insert(listenerClient->getIdentifier(),listenerClient);

            /* send message with confirmation to client */
        }
        else
        {
            if(!masterClient)
            {
                /* create master-client only if no one exist. */
                masterClient = new Master();
//                QObject::connect( masterClient, SIGNAL(receivedSlides()), presentationController, SLOT(onReceivedSlides()) );
//                QObject::connect( xmlReader, SIGNAL(receivedData(unsigned int)), masterClient, SLOT(onReceivedData(unsigned int)) );
                QObject::connect( messageParser, SIGNAL(messageParsed(Message*)), this, SLOT(onMessageParsed(Message *)) );
//                QObject::connect( presentationController, SIGNAL(transmitSlidesResponse(bool)), masterClient, SLOT(transmitSlidesResponse(bool)) );
            }
            else
            {

            }
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
        return masterClient->getIdentifier();
    }

} /* namespace ServerAppl */
