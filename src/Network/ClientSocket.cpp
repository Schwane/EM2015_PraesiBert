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

/**
 * @brief Constructor of the ClientSocket class.
 *
 * Connects the signal <i>connected()</i> of superclass <i>QTcpSocket</i> with the slot <i>handleConnect()</i> of this class.<br>
 * Connects the signal <i>readyRead()</i> of superclass <i>QTcpSocket</i> with the slot <i>handleNewData()</i> of this class.
 */
ClientSocket::ClientSocket(QObject* parent) : QTcpSocket(parent)
{
    connect(this, SIGNAL(connected()), this, SLOT(handleConnect()));
    connect(this, SIGNAL(readyRead()), this, SLOT(handleNewData()));
}

/**
 * @brief Destructor of the ClientSocket class.
 *
 * Closes the socket and deletes itself.
 */
ClientSocket::~ClientSocket()
{
    this->close();
    delete this;
}

/**
 * @brief Method for connecting to a specified IP-Address and port.
 *
 * @param[in] ipAddr_str IP-Address in QString format to connect to.
 * @param[in] port_str Specified port in QString format.
 *
 * @return Returns true, if the connection was established successfully. Otherwise returns false.
 *
 * This method tries to connect to the server at the IP-Address and port that are given as parameter.<br>
 * The method emits the <i>lostConnection</i>-Signal, when the connection could not be established.
 */
bool ClientSocket::connectToServer(QString ipAddr_str, QString port_str)
{
    // Parse Port from String
    int port_int = port_str.toInt(0, 10);

    qDebug() << "Connecting to " << ipAddr_str << "at port " << port_str << ".\n";

    // Connect to designated IP and Port
    connectToHost(ipAddr_str, port_int);

    if(!waitForConnected(5000))
    {
        qDebug() << "Connection to server could not be established.\n";
        lostConnection();
        return false;
    }
    else
        return true;
}

/**
 * @brief Method to disconnect from server.
 *
 * @return Returns true, if disconnecting from the server was successfully.
 *
 * This method lets the socket disconnect from the server that it is connected to.<br>
 * Emits the <i>lostConnection</i>-Signal after it disconnected from the server.
 */
bool ClientSocket::disconnectFromServer()
{
    disconnectFromHost();
    lostConnection();
    qDebug() << "Disconnected from host.\n";
    return true;
}

/**
 * @brief Handler for an established connection.
 *
 * This method is called, when the connection to a server was established successfully.<br>
 * It prints some information to the debug console.
 */
void ClientSocket::handleConnect()
{
    qDebug() << "Established connection to server.\n";
}

/**
 * @brief Handler for new data.
 *
 * This method is called, when new data is available at the client socket.<br>
 * It reads the data from the socket and emits the <i>newData</i>-Signal with the data in QByteArray format.
 */
void ClientSocket::handleNewData()
{
    QByteArray data;
    data = this->readAll();

    qDebug() << "New data at client.\n";
    QString data_str(data);
    qDebug() << data_str << ".\n";

    emit newData(data);
}

/**
 * @brief Slot for sending data.
 *
 * @return Returns the number of sent bytes.
 *
 * Sends the data that is given as parameter in QByteArray format to the server that the client is connected to.
 */
uint ClientSocket::sendData(QByteArray data)
{
    uint sentBytes;
    sentBytes = this->write(data);
    return sentBytes;
}
