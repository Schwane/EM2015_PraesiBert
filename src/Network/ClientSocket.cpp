/*
 * ClientSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "ClientSocket.h"

#include <QDebug>
#include <QHostAddress>
#include <QIODevice>

ClientSocket::ClientSocket(QObject* parent) : QTcpSocket(parent)
{
    connect(this, SIGNAL(connected()), this, SLOT(handleConnect()));
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

    // Connect to designated IP and Port
    this->connectToHost(ipAddr, port_int, QIODevice::ReadWrite);
    qDebug() << "Connected to " << ipAddr_str << "at port " << port_str << ".\n";
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
