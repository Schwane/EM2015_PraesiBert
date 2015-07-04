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
            , m_hasCmdSocket(false)
            , m_hasDataSocket(false)
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
     */
    int ConnectedClient::sendCmd(QByteArray data)
    {
       // Use empty ByteArray "outputData"...
       QByteArray outputData;
       // ...and assign it to outputStream
       QDataStream outputStream(&outputData, QIODevice::ReadWrite);
       outputStream.setVersion(QDataStream::Qt_4_8);
       // Add a 16 bit integer with value 0 to the beginning of the outputStream and add data
       outputStream << quint32(0) << data;
       // Jump to the beginning of the outputStream
       outputStream.device()->seek(0);
       // Add data size at the beginning of the outputStream
       outputStream << (quint32)(outputData.size() - sizeof(quint32));

       // Return value contains actual data size and the size of the quint32 for data length
       return m_cmdSocket->write(outputData);
    }

    /**
     * @brief Sends data to the connected client.
     *
     * @param[in] data Data in QByteArray format that is send.
     *
     * @return Returns the amount of bytes that were actually sent to the client.
     *
     * This method is used to send data to the connected client that is given in QByteArray format as parameter.<br>
     */
    int ConnectedClient::sendData(QByteArray data)
    {
        // Use empty ByteArray "outputData"...
        QByteArray outputData;
        // ... and add it to a the outputStream
        QDataStream outputStream(&outputData, QIODevice::ReadWrite);
        outputStream.setVersion(QDataStream::Qt_4_8);
        // Add a 16 bit integer with value 0 to the beginning of the outputStream and add data
        outputStream << quint32(0) << data;
        // Jump to the beginning of the outputStream
        outputStream.device()->seek(0);
        // Add data size at the beginning of the outputStream
        outputStream << (quint32)(outputData.size() - sizeof(quint32));

        // Return value contains actual data size and the size of the quint32 for data length
        return m_dataSocket->write(outputData);
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
     * @brief This method initializes the sockets.
     *
     * This method is called on start of the QThread that the ConnectedClient object was moved to.<br>
     * It initializes both sockets and connects the signals that are necessary for data handling and connection management.
     */
    void ConnectedClient::process()
    {
        // Check if socket is established already and connect signals and slots
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
    }

    /**
     * @brief Handler for new data from the command socket.
     *
     * This method is called when new data is available at the command socket.<br>
     * It emits the signal <i>newCmd()</i> with the new data in QByteArray format and the clientID.
     */
    void ConnectedClient::handleCmdRead()
    {
        QByteArray inputData;
        // Assign command socket to inputStream
        QDataStream inputStream(m_cmdSocket);

        while(true)
        {
            if(!next_block_size_cmd)
            {
                // Check if block size was received already...
                if(m_cmdSocket->bytesAvailable() < sizeof(quint32))
                    break;
                // ...and set the value to next_block_size_cmd, if so
                inputStream >> next_block_size_cmd;
            }

            if(m_cmdSocket->bytesAvailable() < next_block_size_cmd)
                break;

            // If data is available completely, emit newCmd() signal
            inputStream >> inputData;
            emit newCmd(inputData, m_clientID);

            next_block_size_cmd = 0;
        }
    }

    /**
     * @brief Handler for new data from the data socket.
     *
     * This method is called when new data is available at the data socket.<br>
     * It emits the signal <i>newData()</i> with the new data in QByteArray format and the clientID.
     */
    void ConnectedClient::handleDataRead()
    {
        QByteArray inputData;
        // Assign data socket to inputStream
        QDataStream inputStream(m_dataSocket);

        while(true)
        {
            if(!next_block_size_data)
            {
                // Check if block size was received already...
                if(m_dataSocket->bytesAvailable() < sizeof(quint32))
                    break;
                // ...and set the value to next_block_size_data, if so
                inputStream >> next_block_size_data;
            }

            if(m_dataSocket->bytesAvailable() < next_block_size_data)
                break;

            // If data is available completely, emit receivedData() signal
            inputStream >> inputData;
            emit newData(inputData, m_clientID);

            next_block_size_data = 0;
        }
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

        m_hasCmdSocket = false;
        m_hasDataSocket = false;

        // Call signal that tells which client disconnected
        emit disconnected(m_clientID);
        // Call signal that tells the thread should be finished
        emit finished();
    }
}
