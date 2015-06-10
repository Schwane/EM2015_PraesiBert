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

/**
 * @brief Constructor of the ClientSocket class.
 *
 * Connects the signal <i>connected()</i> of superclass <i>QTcpSocket</i> with the slot <i>handleConnect()</i> of this class.<br>
 * Connects the signal <i>readyRead()</i> of superclass <i>QTcpSocket</i> with the slot <i>handleNewData()</i> of this class.
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
 * Closes the socket and deletes itself.
 */
ClientSocket::~ClientSocket()
{
    m_cmdSocket->close();
    m_dataSocket->close();
    delete m_cmdSocket;
    delete m_dataSocket;
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
bool ClientSocket::connectToServer(QString ipAddr_str, QString cmdPort_str, QString dataPort_str)
{
    // Parse Ports from Strings
    int cmdPort_int = cmdPort_str.toInt(0, 10);
    int dataPort_int = dataPort_str.toInt(0, 10);

    qDebug() << "Connecting to Server at " << ipAddr_str << ".\n";
    qDebug() << "Command port: " << cmdPort_str << ".\n" << "Data port: " << dataPort_str << ".\n";

    // Connect to designated IP and Port
    m_cmdSocket->connectToHost(ipAddr_str, cmdPort_int);
    m_dataSocket->connectToHost(ipAddr_str, dataPort_int);

    if(!m_cmdSocket->waitForConnected(5000) | !m_dataSocket->waitForConnected(5000))
    {
        qDebug() << "Connection to Server could not be established.\n";
        emit lostConnection();
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
void ClientSocket::disconnectFromServer()
{
    m_cmdSocket->disconnectFromHost();
    m_dataSocket->disconnectFromHost();

    m_cmdSocket->close();
    m_dataSocket->close();

    emit lostConnection();
    qDebug() << "Disconnected from host.\n";
}

/**
 * @brief Handler for new data.
 *
 * This method is called, when new data is available at the client socket.<br>
 * It reads the data from the socket and emits the <i>newData</i>-Signal with the data in QByteArray format.
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

int ClientSocket::sendData(QByteArray data, int connectionType)
{
    switch(connectionType)
    {
        case ClientSocket::cmdConnection:
            return m_cmdSocket->write(data);
        case ClientSocket::dataConnection:
            return m_dataSocket->write(data);
        default:
            return 0;
    }
}
