/*
 * ConnectedClient.cpp
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#include "ConnectedClient.h"

ConnectedClient::ConnectedClient(int socketDescriptor, unsigned int clientID)
        : socketDescriptor(socketDescriptor)
        , clientID(clientID)
{
}

ConnectedClient::~ConnectedClient()
{
    tcpSocket->close();
    delete tcpSocket;
}

void ConnectedClient::process()
{
    // Create socket object and set socket descriptor on start of thread
    tcpSocket = new QTcpSocket(this);
    tcpSocket->setSocketDescriptor(socketDescriptor);

    // Connect signals and slots for event handling
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
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
    // Call signal that tells which client disconnected
    emit disconnected(clientID);
    // Call signal that tells the thread should be finished
    emit finished();
}
