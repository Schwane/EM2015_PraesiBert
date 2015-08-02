/*
 * ConnectedClient.cpp
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#include "include/ConnectedClient.h"

#include <QDebug>
#include <QHostAddress>
#include <QIODevice>

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
            , m_hasCmdSocket(false)
            , m_hasDataSocket(false)
    {
        m_next_block_size_cmd = 0;
        m_next_block_size_data = 0;
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
     * @brief Method used to set the command socket.
     *
     * @param[in] tcpSocket New socket object that one of the clients sockets is assigned to.
     *
     * Each time a new connection is established with the server, a new QTcpSocket object is created.<br>
     * This object is then passed to this function as parameter.<br>
     * Within this function, the new socket object is then assigned to the command socket.
     */
    void ConnectedClient::setCmdSocket(QTcpSocket* tcpSocket)
    {
        m_cmdSocket = tcpSocket;
        connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleCmdRead()));
        connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        m_hasCmdSocket = true;
        return;
    }

    /**
     * @brief Method used to set the data socket.
     *
     * @param[in] tcpSocket New socket object that one of the clients sockets is assigned to.
     *
     * Each time a new connection is established with the server, a new QTcpSocket object is created.<br>
     * This object is then passed to this function as parameter.<br>
     * Within this function, the new socket object is then assigned to the data socket.
     */
    void ConnectedClient::setDataSocket(QTcpSocket* tcpSocket)
    {
        m_dataSocket = tcpSocket;
        connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
        connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        m_hasDataSocket = true;
        return;
    }

    /**
     * @brief Method that returns, whether the command socket is established already.
     *
     * @return Returns true, if the socket is set up and available.
     */
    bool ConnectedClient::hasCmdSocket()
    {
        return m_hasCmdSocket;
    }

    /**
     * @brief Method that returns, whether the data socket is established already.
     *
     * @return Returns true, if the socket is set up and available.
     */
    bool ConnectedClient::hasDataSocket()
    {
        return m_hasDataSocket;
    }

    /**
     * @brief Sends a command to the connected client.
     *
     * @param[in] data Command in QByteArray format that is send.
     *
     * @return Returns the amount of bytes that were actually sent to the client.
     *
     * This method is used to send a command to the connected client that is given in QByteArray format as parameter.<br>
     * A 32 bit integer with information about the data length is send first. Then the actual data follows.
     */
    int ConnectedClient::sendCmd(QByteArray data)
    {
        // Return value contains actual data size and the size of the quint32 for data length
        quint32 size = data.size();
        QByteArray size_arr;
        for(int i = 0; i < 4; ++i)
        {
            size_arr.append((char)((size & (0xFF << (i*8))) >> (i*8)));
        }
        data.insert(0, size_arr);
        return m_cmdSocket->write(data);
    }

    /**
     * @brief Sends data to the connected client.
     *
     * @param[in] data Data in QByteArray format that is send.
     *
     * @return Returns the amount of bytes that were actually sent to the client.
     *
     * This method is used to send data to the connected client that is given in QByteArray format as parameter.<br>
     * A 32 bit integer with information about the data length is send first. Then the actual data follows.
     */
    int ConnectedClient::sendData(QByteArray data)
    {
        // Return value contains actual data size and the size of the quint32 for data length
        quint32 size = data.size();
        QByteArray size_arr;
        for(int i = 0; i < 4; ++i)
        {
            size_arr.append((char)((size & (0xFF << (i*8))) >> (i*8)));
        }
        data.insert(0, size_arr);
        return m_dataSocket->write(data);
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
        if(m_hasCmdSocket)
        {
            return m_cmdSocket->peerAddress();
        }
        else
        {
            if(m_hasDataSocket)
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
     * @brief Method is called on start of the Thread.
     *
     * This method is called on start of the QThread that the ConnectedClient object was moved to.
     */
    void ConnectedClient::process()
    {
        // Check if socket is established already and connect signals and slots
        /*
        if(m_hasCmdSocket)
        {
            connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleCmdRead()));
            connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        }
        if(m_hasDataSocket)
        {
            connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(handleDataRead()));
            connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
        }
        */
    }

    /**
     * @brief Handler for new data from the command socket.
     *
     * This method is called when new data is available at the command socket.<br>
     * A 32 bit integer is used to determine the length of the receiving data.<br>
     * This method is returned without reading data from the socket, until the expected amount of data is available at the socket.<br>
     * The function then emits the signal <i>newCmd</i> with the complete received command in QByteArray format.<br>
     * It reads data from the socket, until there is no more data available.
     */
    void ConnectedClient::handleCmdRead()
    {
        QByteArray inputData;

        while (1)
        {
        if (!m_next_block_size_cmd)
        {
            int x = 4;

            while (x > 0)
            {
                inputData = m_cmdSocket->read(x);
                x -= inputData.size();
            }

            for(int i = 0; i < 4; ++i)
            {
                m_next_block_size_cmd |= ((quint8) inputData.at(i)) << i * 8;
            }
        }

        if (m_cmdSocket->bytesAvailable() < m_next_block_size_cmd)
            return;
        else
        {
            inputData.clear();
            inputData = m_cmdSocket->read(qMin(m_next_block_size_cmd, m_cmdSocket->bytesAvailable()));
            m_next_block_size_cmd = 0;
            m_bufferCmd.append(inputData);
            qDebug() << "data: " << m_bufferCmd;
        }

        if (m_next_block_size_cmd == 0)
        {
            qDebug() << "New command at client.\n" << m_bufferCmd;
            emit newCmd(m_bufferCmd, m_clientID);
            m_bufferCmd.clear();
        }

            if (m_cmdSocket -> bytesAvailable() == 0)
                break;
        }
        return;
    }

    /**
     * @brief Handler for new data from the data socket.
     *
     * This method is called when new data is available at the data socket.<br>
     * A 32 bit integer is used to determine the length of the receiving data.<br>
     * This method is returned without reading data from the socket, until the expected amount of data is available at the socket.<br>
     * The function then emits the signal <i>newData</i> with the complete received command in QByteArray format.<br>
     * It reads data from the socket, until there is no more data available.
     */
    void ConnectedClient::handleDataRead()
    {
        QByteArray inputData;
        while(1)
        {
        if (!m_next_block_size_data)
        {
            int x = 4;

            while (x > 0)
            {
                inputData = m_dataSocket->read(x);
                x -= inputData.size();
            }

            for(int i = 0; i < 4; ++i)
            {
                m_next_block_size_data |= ((quint8) inputData.at(i)) << i * 8;
            }
        }

        if (m_dataSocket->bytesAvailable() < m_next_block_size_data)
            return;
        else
        {
            inputData.clear();
            inputData = m_dataSocket->read(qMin(m_next_block_size_data, m_dataSocket->bytesAvailable()));
            m_next_block_size_data = 0;
            m_bufferData.append(inputData);
            qDebug() << "data: " << m_bufferData;
        }

        if (m_next_block_size_data == 0)
        {
            emit newData(m_bufferData, m_clientID);
            qDebug() << "New data at client.\n" << m_bufferData;
            m_bufferData.clear();
        }
        if (m_dataSocket -> bytesAvailable() == 0)
                    break;
        }
        return;
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

//        disconnect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
//        disconnect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));

        m_cmdSocket->close();
        m_dataSocket->close();

        m_hasCmdSocket = false;
        m_hasDataSocket = false;

        // Call signal that tells which client disconnected
        emit disconnected(m_clientID);
        // Call signal that tells the thread should be finished
        emit finished();
    }
}
