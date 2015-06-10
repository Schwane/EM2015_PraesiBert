/*
 * ConnectedClient.cpp
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#include "include/ConnectedClient.h"

namespace Network
{
    /**
     * @brief Constructor of the ConnectedClient class.
     *
     * @param[in] clientID ID of the client that is created.
     *
     * Initializes the variables <i>m_clientID</i> with the value that was given as parameter.<br>
     * Also initializes the booleans <i>hasCmdSocket</i> and <i>hasDataSocket</i> with the value <i>false</i>.
     * The constructor does not implement a parent object so that it can be moved into a QThread.
     */
    ConnectedClient::ConnectedClient(uint clientID)
            : m_clientID(clientID)
            , hasCmdSocket(false)
            , hasDataSocket(false)
    {
    }

    /**
     * @brief Destructor of the ConnectedClient class.
     *
     * The destructor closes the sockets and deletes them.
     */
    ConnectedClient::~ConnectedClient()
    {
        m_cmdSocket->close();
        m_dataSocket->close();
        delete m_cmdSocket;
        delete m_dataSocket;
    }

    /**
     * @brief Method used to set one of the sockets.
     *
     * @param[in] tcpSocket New socket object that one of the clients sockets is assigned to.
     * @param[in] connectionType Defines, which of the sockets is set to the new socket.
     *
     * Each time a new connection is established with the server, a new QTcpSocket object is created.<br>
     * This object is then passed to this function with the <i>connectionType</i> as parameter.<br>
     * Within this function the new socket object is then assigned to the command socket or data socket, depending on the <i>connectionType</i>.
     */
    void ConnectedClient::setSocket(QTcpSocket* tcpSocket, int connectionType)
    {
        if(connectionType == ConnectedClient::cmdConnection)
        {
            m_cmdSocket = tcpSocket;
            connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleCmdRead()));
            connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
            hasCmdSocket = true;
            return;
        }
        if(connectionType == ConnectedClient::dataConnection)
        {
            m_dataSocket = tcpSocket;
            connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
            connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
            hasDataSocket = true;
            return;
        }
    }

    /**
     * @brief Method that returns, if a type of socket is established already.
     *
     * @param[in] connectionType Defines the type of socket that is requested.
     *
     * @return Returns true, if the socket that was requested with the <i>connectionType</i> is set up and available.
     */
    bool ConnectedClient::hasSocketType(int connectionType)
    {
        if(connectionType == ConnectedClient::cmdConnection)
        {
            return hasCmdSocket;
        }
        if(connectionType == ConnectedClient::dataConnection)
        {
            return hasDataSocket;
        }
        // Return false if none of the above statements is true
        return false;
    }

    /**
     * @brief Sends data to the connected client.
     *
     * @param[in] data Data in QByteArray format that is send.
     * @param[in] connectionType Defines from which socket the data is send.
     *
     * @return Returns the amount of bytes that were actually send to the client.
     *
     * This method is used to send data to the connected client that is given in QByteArray format as parameter.<br>
     * The socket that is used to send the data is define with the parameter <i>connectionType</i>.
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
     * This class closes the connection to the servers sockets.
     */
    void ConnectedClient::disconnectFromServer()
    {
        m_cmdSocket->disconnectFromHost();
        m_dataSocket->disconnectFromHost();
        // Do not emit signal finished() here! Will be done in handleDisconnect()!
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
     * @brief Returns the peer address of the connected client.
     *
     * @return Returns the peer address of the connected client in QHostAddress format.
     *
     * This function returns the peer address of the connected client.<br>
     * The command socket is primarily used to determine the peer address.<br>
     * If the command socket is not available, the data socket is used.<br>
     * If none of the sockets is available, localhost is returned as peer address.
     */
    QHostAddress ConnectedClient::getPeerAddress()
    {
        // Use Command-Socket for verifying the peer address primarily
        if(hasCmdSocket)
        {
            return m_cmdSocket->peerAddress();
        }
        else
        {
            if(hasDataSocket)
            {
                return m_dataSocket->peerAddress();
            }
            else
            {
                return QHostAddress::LocalHost;
            }
        }
    }

    /**
     * @brief This method initializes the sockets.
     *
     * This method is called on start of the QThread that the ConnectedClient object was moved to.<br>
     * It initializes both sockets and connects the signals that are necessary for data handling and connection management.
     */
    void ConnectedClient::process()
    {
        // Check if socket is established already and connect signals and slots
        if(hasCmdSocket)
        {
            connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleCmdRead()));
            connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        }
        if(hasDataSocket)
        {
            connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
            connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        }
    }

    /**
     * @brief Handler for new data from the command socket.
     *
     * This method is called when new data is available at the command socket.<br>
     * It emits the signal <i>newData()</i> with the new data in QByteArray format, the clientID and the connectionType <i>ConnectedClient::cmdConnection</i>.
     */
    void ConnectedClient::handleCmdRead()
    {
        QByteArray readData = m_cmdSocket->readAll();
        emit newData(readData, m_clientID, ConnectedClient::cmdConnection);
    }

    /**
     * @brief Handler for new data from the data socket.
     *
     * This method is called when new data is available at the data socket.<br>
     * It emits the signal <i>newData()</i> with the new data in QByteArray format, the clientID and the connectionType <i>ConnectedClient::dataConnection</i>.
     */
    void ConnectedClient::handleDataRead()
    {
        QByteArray readData = m_dataSocket->readAll();
        emit newData(readData, m_clientID, ConnectedClient::dataConnection);
    }

    /**
     * @brief Disconnect handler for the client.
     *
     * This method is called, when one of the clients sockets was disconnected from the server.<br>
     * It closes the sockets, emits the signal <i>disconnected()</i> with its clientID and emits the signal <i>finished()</i>.
     */
    void ConnectedClient::handleDisconnect()
    {
        // Close BOTH socket types, if one connection is lost!
        m_cmdSocket->close();
        m_dataSocket->close();

        hasCmdSocket = false;
        hasDataSocket = false;

        // Call signal that tells which client disconnected
        emit disconnected(m_clientID);
        // Call signal that tells the thread should be finished
        emit finished();
    }
}
