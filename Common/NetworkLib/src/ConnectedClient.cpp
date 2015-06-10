/*
 * ConnectedClient.cpp
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#include "include/ConnectedClient.h"

/**
 * @brief Constructor of the ConnectedClient class.
 *
 * Initializes the variables <i>m_socketDescriptor</i> and <i>m_clientID</i> with the values that were given as parameters.<br>
 * The constructor does not implement a parent object so that it can be moved into a QThread.
 */
ConnectedClient::ConnectedClient(uint clientID)
        : m_clientID(clientID)
        , isCmdSocketInit(false)
        , isDataSocketInit(false)
{
}

/**
 * @brief Destructor of the ConnectedClient class.
 *
 * The destructor closes and deletes the socket.
 */
ConnectedClient::~ConnectedClient()
{
    m_cmdSocket->close();
    m_dataSocket->close();
    delete m_cmdSocket;
    delete m_dataSocket;
}

/**
 * @brief This method initializes the QTcpSocket.
 *
 * This method is called on start of the QThread that the ConnectedClient object was moved to.<br>
 * It initializes the QTcpSocket and connects the signals that are necessary for data handling and connection management.
 */
void ConnectedClient::process()
{
    // Check of socket is established already and connect signals and slots
    if(isCmdSocketInit)
    {
        connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleCmdRead()));
        connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
    }
    if(isDataSocketInit)
    {
        connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
        connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
    }
}

/**
 * @brief Returns the clientID of the socket.
 *
 * @return Returns the clientID of the socket.
 */
uint ConnectedClient::getClientID()
{
    return m_clientID;
}

QHostAddress ConnectedClient::getPeerAddress()
{
    // Use Command-Socket for verifying the peer address primarily
    if(isCmdSocketInit)
    {
        return m_cmdSocket->peerAddress();
    }
    else
    {
        if(isDataSocketInit)
        {
            return m_dataSocket->peerAddress();
        }
        else
        {
            return QHostAddress::LocalHost;
        }
    }
}

void ConnectedClient::setSocket(QTcpSocket* tcpSocket, int connectionType)
{
    if(connectionType == ConnectedClient::cmdConnection)
    {
        m_cmdSocket = tcpSocket;
        connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleCmdRead()));
        connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        isCmdSocketInit = true;
        return;
    }
    if(connectionType == ConnectedClient::dataConnection)
    {
        m_dataSocket = tcpSocket;
        connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
        connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        isDataSocketInit = true;
        return;
    }
}

bool ConnectedClient::hasSocketType(int connectionType)
{
    if(connectionType == ConnectedClient::cmdConnection)
    {
        return isCmdSocketInit;
    }
    if(connectionType == ConnectedClient::dataConnection)
    {
        return isDataSocketInit;
    }
    // Return false if none of the above statements is true
    return false;
}

void ConnectedClient::handleCmdRead()
{
    QByteArray readData = m_cmdSocket->readAll();
    emit newData(readData, m_clientID, ConnectedClient::cmdConnection);
}

/**
 * @brief Handler for new data at socket.
 *
 * This method is called when new data is available at the socket.<br>
 * It emits the signal <i>newData</i> with the new data in QByteArray format.
 */
void ConnectedClient::handleDataRead()
{
    QByteArray readData = m_dataSocket->readAll();
    emit newData(readData, m_clientID, ConnectedClient::dataConnection);
}

/**
 * @brief Sends data to the connected client.
 *
 * @return Returns the amount of bytes that were actually send to the client.
 *
 * This method is used to send data to the connected client that is given in QByteArray format as parameter.<br>
 */
int ConnectedClient::sendData(QByteArray data, int connectionType)
{
    if(connectionType == ConnectedClient::cmdConnection)
    {
        qint64 sentBytes = m_cmdSocket->write(data);
        return sentBytes;
    }
    if(connectionType == ConnectedClient::dataConnection)
    {
        qint64 sentBytes = m_dataSocket->write(data);
        return sentBytes;
    }
    // Return 0 of none of socket was selected correctly
    return 0;
}

/**
 * @brief Closes the connection to the Server.
 *
 * This class closes the connection to the server.
 * It emits the signal <i>finished</i> to quit the thread.
 */
void ConnectedClient::disconnectFromServer()
{
    m_cmdSocket->disconnectFromHost();
    m_dataSocket->disconnectFromHost();
    emit finished();
}

/**
 * @brief Disconnect handler for the client.
 *
 * This method is called, when the client was disconnected from the server.<br>
 * It closes the socket, emits the signal <i>disconnected</i> with it's clientID and emits the signal <i>finished</i>.
 */
void ConnectedClient::handleDisconnect()
{
    // Close BOTH socket types, if one connection is lost!
    m_cmdSocket->close();
    m_dataSocket->close();

    isCmdSocketInit = false;
    isDataSocketInit = false;

    // Call signal that tells which client disconnected
    emit disconnected(m_clientID);
    // Call signal that tells the thread should be finished
    emit finished();
}
