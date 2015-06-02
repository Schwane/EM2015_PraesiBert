/*
 * ConnectedClient.cpp
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#include "ConnectedClient.h"

/**
 * @brief Constructor of the ConnectedClient class.
 *
 * Initializes the variables <i>m_socketDescriptor</i> and <i>m_clientID</i> with the values that were given as parameters.<br>
 * The constructor does not implement a parent object so that it can be moved into a QThread.
 */
ConnectedClient::ConnectedClient(int socketDescriptor, uint clientID)
        : m_socketDescriptor(socketDescriptor)
        , m_clientID(clientID)
{
}

/**
 * @brief Destructor of the ConnectedClient class.
 *
 * The destructor closes and deletes the socket.
 */
ConnectedClient::~ConnectedClient()
{
    m_tcpSocket->close();
    delete m_tcpSocket;
}

/**
 * @brief This method initializes the QTcpSocket.
 *
 * This method is called on start of the QThread that the ConnectedClient object was moved to.<br>
 * It initializes the QTcpSocket and connects the signals that are necessary for data handling and connection management.
 */
void ConnectedClient::process()
{
    // Create socket object and set socket descriptor on start of thread
    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->setSocketDescriptor(m_socketDescriptor);

    // Connect signals and slots for event handling
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
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

/**
 * @brief Handler for new data at socket.
 *
 * This method is called when new data is available at the socket.<br>
 * It emits the signal <i>newData</i> with the new data in QByteArray format.
 */
void ConnectedClient::handleDataRead()
{
    QByteArray readData = m_tcpSocket->readAll();
    emit newData(readData, m_clientID);
}

/**
 * @brief Sends data to the connected client.
 *
 * @return Returns the amount of bytes that were actually send to the client.
 *
 * This method is used to send data to the connected client that is given in QByteArray format as parameter.<br>
 */
int ConnectedClient::sendData(QByteArray data)
{
    qint64 sentBytes = m_tcpSocket->write(data);
    return sentBytes;
}

/**
 * @brief Closes the connection to the Server.
 *
 * This class closes the connection to the server.
 * It emits the signal <i>finished</i> to quit the thread.
 */
void ConnectedClient::disconnectFromServer()
{
    m_tcpSocket->disconnectFromHost();
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
    m_tcpSocket->close();
    // Call signal that tells which client disconnected
    emit disconnected(m_clientID);
    // Call signal that tells the thread should be finished
    emit finished();
}
