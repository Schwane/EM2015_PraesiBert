/*
 * ClientSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "include/ClientSocket.h"

// Qt includes
#include <QDebug>
#include <QHostAddress>
#include <QIODevice>

namespace Network
{
    /**
     * @brief Constructor of the ClientSocket class.
     *
     * Initializes the command and data socket and connects signals and slots for connection and data handling.<br>
     * Connects the signal <i>connected()</i> of the sockets with the handlers (<i>connectedToCmdServer()</i> and (<i>connectedToDataServer()</i>) of this class.<br>
     * Connects the signal <i>disconnected()</i> of the sockets with the slot <i>disconnectFromServer()</i> of this class.<br>
     * Connects the signal <i>readyRead()</i> of the command socket with the slot <i>handleNewCmd()</i> of this class.
     * Connects the signal <i>readyRead()</i> of the data socket with the slot <i>handleNewData()</i> of this class.
     */
    ClientSocket::ClientSocket(QObject* parent) : QObject(parent)
    {
        m_cmdSocket = new QTcpSocket(parent);
        m_dataSocket = new QTcpSocket(parent);

        m_next_block_size_cmd = 0;
        m_next_block_size_data = 0;

        connect(m_cmdSocket, SIGNAL(connected()), this, SIGNAL(connectedToCmdServer()));
        connect(m_dataSocket, SIGNAL(connected()), this, SIGNAL(connectedToDataServer()));

        connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));
        connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));

        connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleNewCmd()));
        connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(handleNewData()));
    }

    /**
     * @brief Destructor of the ClientSocket class.
     *
     * Closes the sockets and deletes them.
     */
    ClientSocket::~ClientSocket()
    {
        m_cmdSocket->close();
        m_dataSocket->close();
        delete m_cmdSocket;
        delete m_dataSocket;
    }

    /**
     * @brief Method for connecting the sockets to a server at the specified IP-Address.
     *
     * @param[in] ipAddr_str IP-Address in QString format to connect to.
     * @param[in] cmdPort_str Specified port for the command connection in QString format.
     * @param[in] dataPort_str Specified port for the data connection in QString format.
     *
     * @return Returns true, if the connection was established successfully. Otherwise returns false.
     *
     * This method tries to connect to the server sockets at the IP-Address and the ports that were given as parameters.<br>
     * The method calls the disconnectFromServer() method, if the connection cannot be established.
     */
    bool ClientSocket::connectToServer(QString ipAddr_str, QString cmdPort_str, QString dataPort_str)
    {
        int cmdPort_int = cmdPort_str.toInt(0, 10);
        int dataPort_int = dataPort_str.toInt(0, 10);

        qDebug() << "Connecting to Server at " << ipAddr_str << ".\n";
        qDebug() << "Command port: " << cmdPort_str << ".\n" << "Data port: " << dataPort_str << ".\n";

        m_cmdSocket->connectToHost(ipAddr_str, cmdPort_int);
        m_dataSocket->connectToHost(ipAddr_str, dataPort_int);

        // Close the sockets, if one of the connections could not be established with the server
        if(!m_cmdSocket->waitForConnected(5000) | !m_dataSocket->waitForConnected(5000))
        {
            qDebug() << "Connection to Server could not be established.\n";
            disconnectFromServer();
            return false;
        }
        else
            return true;
    }

    /**
     * @brief Method to disconnect from server.
     *
     * This method lets the socket disconnect from the server that it is connected to.<br>
     * Emits the <i>lostConnection</i>-Signal after it disconnected from the server.
     */
    void ClientSocket::disconnectFromServer()
    {
        m_cmdSocket->disconnectFromHost();
        m_dataSocket->disconnectFromHost();

        m_cmdSocket->close();
        m_dataSocket->close();

        emit lostConnection();
        qDebug() << "Closed connection to host.\n";
    }

    /**
     * @brief Method that is used to send a command from the command socket.
     *
     * @param[in] data Command that is send to the server.
     *
     * @return Returns the number of bytes that were actually send to the server.
     *
     * This method sends data from the command socket to the server.<br>
     * A 32 bit integer with information about the data length is send first. Then the actual data follows.
     */
    int ClientSocket::sendCmd(QByteArray data)
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
     * @brief Method that is used to send data from the data socket.
     *
     * @param[in] data Data that is send to the server.
     *
     * @return Returns the number of bytes that were actually send to the server.
     *
     * This method sends data from the data socket to the server.<br>
     * A 32 bit integer with information about the data length is send first. Then the actual data follows.
     */
    int ClientSocket::sendData(QByteArray data)
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
     * @brief Handler for new command.
     *
     * This method is called, when a new command is available at the command socket.<br>
     * A 32 bit integer is used to determine the length of the receiving data.<br>
     * This method is returned without reading data from the socket, until the expected amount of data is available at the socket.<br>
     * The function then emits the signal <i>receivedCmd</i> with the complete received command in QByteArray format.<br>
     * It reads data from the socket, until there is no more data available.
     */
    void ClientSocket::handleNewCmd()
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
            emit receivedCmd(m_bufferCmd);
            qDebug() << "New command at client.\n" << m_bufferCmd;
            m_bufferCmd.clear();
        }
        if (m_cmdSocket -> bytesAvailable() == 0)
            break;
        }
        return;
    }

    /**
     * @brief Handler for new data.
     *
     * This method is called, when new data is available at the data socket.<br>
     * A 32 bit integer is used to determine the length of the receiving data.<br>
     * This method is returned without reading data from the socket, until the expected amount of data is available at the socket.<br>
     * The function then emits the signal <i>receivedData</i> with the complete received command in QByteArray format.<br>
     * It reads data from the socket, until there is no more data available.
     */
    void ClientSocket::handleNewData()
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
            emit receivedData(m_bufferData);
            qDebug() << "New data at client.\n" << m_bufferData;
            m_bufferData.clear();
        }
        if (m_dataSocket -> bytesAvailable() == 0)
                    break;
        }
        return;
    }
}
