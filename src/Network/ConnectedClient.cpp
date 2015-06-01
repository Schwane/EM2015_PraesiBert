/*
 * ConnectedClient.cpp
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#include "ConnectedClient.h"

// Qt includes
#include <QByteArray>

ConnectedClient::ConnectedClient(QObject* parent, int socketDescriptor, unsigned int clientID)
        : QThread(parent)
        , socketDescriptor(socketDescriptor)
        , clientID(clientID)
{
}

ConnectedClient::~ConnectedClient()
{
    tcpSocket->close();
    delete tcpSocket;
}

void ConnectedClient::run()
{
    // Create socket object and set socket descriptor on start of thread
    // Do not set "this" as parent for socket. Leads to critical crash.
    tcpSocket = new QTcpSocket();
    tcpSocket->setSocketDescriptor(socketDescriptor);

    // Connect signals and slots for event handling
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));

    // Enter the event loop
    exec();
}

unsigned int ConnectedClient::getClientID()
{
    return clientID;
}

void ConnectedClient::handleDataRead()
{
    QByteArray readData = tcpSocket->readAll();
    emit newData(readData, clientID);
}

int ConnectedClient::sendData(QByteArray data)
{
    qint64 sentBytes = tcpSocket->write(data);
    return sentBytes;
}

void ConnectedClient::handleDisconnect()
{
    tcpSocket->close();
    tcpSocket->deleteLater();
    emit disconnected(clientID);
}
