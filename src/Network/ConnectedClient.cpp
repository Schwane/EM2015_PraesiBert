/*
 * ConnectedClient.cpp
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#include <src/Network/ConnectedClient.h>

// Qt includes
#include <QByteArray>

ConnectedClient::ConnectedClient(QObject* parent, int socketDescriptor, unsigned int clientID)
        : QThread(parent)
        , clientID(clientID)
{
    tcpSocket = new QTcpSocket(this);
    // Set socketdescriptor on creation of object
    tcpSocket->setSocketDescriptor(socketDescriptor);

    // Connect signals and slots for event handling
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
}

ConnectedClient::~ConnectedClient()
{
    tcpSocket->close();
    delete tcpSocket;
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
