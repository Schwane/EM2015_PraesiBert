/*
 * ServerSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "ServerSocket.h"

#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QList>

ServerSocket::ServerSocket(QObject* parent) : QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(handleClientConnection()));
}

ServerSocket::~ServerSocket()
{
    this->close();
    delete this;
}

bool ServerSocket::beginListening(QString port_str)
{
    // Define variables for IP-Address and Port
    QString ipAddress_str;

    // Fetch available IP-Addresses
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();

    // Start listening for incoming connections on any address
    if(!this->listen(QHostAddress::Any, port_str.toInt(0, 10)))
    {
        // Close socket and return Error-State, if listening failed
        qDebug() << "Could not start the Server. Closing socket.\n";
        this->close();
        return false;
    }

    // Loop through list of available IPs and choose the first IPv4 that is not localhost
    for (int i = 0; i < ipList.size(); ++i) {
        if (ipList.at(i) != QHostAddress::LocalHost && ipList.at(i).toIPv4Address())
        {
            ipAddress_str = ipList.at(i).toString();
            break;
        }
    }

    // If no IP was found, use localhost
    if (ipAddress_str.isEmpty())
        ipAddress_str = QHostAddress(QHostAddress::LocalHost).toString();

    // Emit signal with new IP-Address of the server
    emit newIP(ipAddress_str);

    qDebug() << "Listening for incoming Clients on Port " << port_str << ".\n IP-Address is: " << ipAddress_str << ".\n";
    return true;
}

bool ServerSocket::closeServer()
{
    this->close();
    return true;
}

bool ServerSocket::send()
{
    // TODO: Implement send method
    return true;
}

bool ServerSocket::handleClientConnection()
{
    aTcpConnection = this->nextPendingConnection();
    qDebug() << "New connection to server";
    return true;
}

bool ServerSocket::handleClientDisconnect()
{
    return true;
}
