/*
 * ClientSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "ClientSocket.h"

// Qt includes
#include <QDebug>
#include <QHostAddress>
#include <QIODevice>

ClientSocket::ClientSocket(QObject* parent) : QTcpSocket(parent)
{
    connect(this, SIGNAL(connected()), this, SLOT(handleConnect()));
    connect(this, SIGNAL(readyRead()), this, SLOT(handleNewData()));
}

ClientSocket::~ClientSocket()
{
    this->close();
    delete this;
}

bool ClientSocket::connectToServer(QString ipAddr_str, QString port_str)
{
    // Parse IP and Port from Strings
    QHostAddress ipAddr(ipAddr_str);
    int port_int = port_str.toInt(0, 10);

    qDebug() << "Connecting to " << ipAddr_str << "at port " << port_str << ".\n";

    // Connect to designated IP and Port
    this->connectToHost(ipAddr, port_int, QIODevice::ReadWrite);
    return true;
}

bool ClientSocket::disconnectFromServer()
{
    this->disconnectFromHost();
    qDebug() << "Disconnected from host.\n";
    return true;
}

bool ClientSocket::handleConnect()
{
    qDebug() << "Established connection to server.\n";
    return true;
}

bool ClientSocket::handleNewData()
{
    QByteArray data;
    data = this->readAll();

    qDebug() << "New data at client.\n";
    QString data_str(data);
    qDebug() << data_str << "\n.";

    emit newData(data);

    return true;
}

int ClientSocket::sendData(QByteArray data)
{
    unsigned int sentBytes;
    sentBytes = this->write(data);
    return sentBytes;
}
