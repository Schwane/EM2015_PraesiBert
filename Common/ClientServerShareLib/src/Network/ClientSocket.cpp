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

namespace Network
{
    /**
     * @brief Constructor of the ClientSocket class.
     *
     * Initializes the command and data socket and connects signals and slots for connection and data handling.<br>
     * Connects the signal <i>connected()</i> of the sockets with the handlers (<i>connectedToCmdServer()</i> and (<connectedToDataServer()</i>) of this class.<br>
     * Connects the signal <i>disconnected()</i> of the sockets with the slot <i>disconnectFromServer()</i> of this class.<br>
     * Connects the signal <i>readyRead()</i> of the sockets with the slot <i>handleNewData()</i> of this class.
     */
    ClientSocket::ClientSocket(QObject* parent) : QObject(parent)
    {
        m_cmdSocket = new QTcpSocket(parent);
        m_dataSocket = new QTcpSocket(parent);

        connect(m_cmdSocket, SIGNAL(connected()), this, SIGNAL(connectedToCmdServer()));
        connect(m_dataSocket, SIGNAL(connected()), this, SIGNAL(connectedToDataServer()));

        connect(m_cmdSocket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));
        connect(m_dataSocket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));

        connect(m_cmdSocket, SIGNAL(readyRead()), this, SLOT(handleNewData()));
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
     * @brief Method that is used to send data from one of the sockets.
     *
     * @param[in] data Command that is send to the server.
     *
     * @return Returns the number of bytes that were actually send to the server.
     *
     * This method sends data from the command socket to the server.<br>
     */
    int ClientSocket::sendCmd(QByteArray data)
    {
        return m_cmdSocket->write(data);
    }

    /**
     * @brief Method that is used to send data from one of the sockets.
     *
     * @param[in] data Data that is send to the server.
     *
     * @return Returns the number of bytes that were actually send to the server.
     *
     * This method sends data from the data socket to the server.<br>
     */
    int ClientSocket::sendData(QByteArray data)
    {
        return m_dataSocket->write(data);
    }

    /**
     * @brief Handler for new data.
     *
     * This method is called, when new data is available at the client socket.<br>
     * The method determines which socket sent the signal and reads the data from that socket.<br>
     * Depending on the socket that the data was read from, either the signal <i>receivedCmd</i> or <i>receivedData</i> is emitted.
     */
    void ClientSocket::handleNewData()
    {
        QTcpSocket* signalSender;
        signalSender = qobject_cast<QTcpSocket*>(sender());

        QByteArray data;

        if(m_cmdSocket == signalSender)
        {
            data = m_cmdSocket->readAll();
            emit receivedCmd(data);
            qDebug() << "New command at client.\n";
            QString data_str(data);
            qDebug() << data_str << ".\n";
            return;
        }
        if(m_dataSocket == signalSender)
        {
            data = m_cmdSocket->readAll();
            emit receivedData(data);
            qDebug() << "New data at client.\n";
            QString data_str(data);
            qDebug() << data_str << ".\n";
            return;
        }
    }
}
