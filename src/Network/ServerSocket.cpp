/*
 * ServerSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "ServerSocket.h"

// QT includes
#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QList>

/**
 * @brief Constructor of the ServerSocket class.
 *
 * @param[in] parent Parent QObject that creates the server socket object.
 *
 * TODO: Obsolete!?
 * The constructor connects signals of the <i>QTcpServer</i> class with slots of the ServerSocket class.<br>
 * These connections include handling for:<br>
 * <ul>
 *  <li>New connections of clients (method: ServerSocket::handleClientConnection())</li>
 * </ul>
 */
ServerSocket::ServerSocket(QObject* parent)
        : QTcpServer(parent)
        , connectedClients(0)
        , clientID(0)
{
}

/**
 * @brief Destructor of the ServerSocket class.
 *
 * Closes the server socket and deletes itself.
 */
ServerSocket::~ServerSocket()
{
    this->closeServer();
    delete this;
}

/**
 * @brief Method that is called to start listening for incoming connections.
 *
 * @param[in] port_str Listening port for incoming connections in QString format.
 *
 * @return Returns true, if the listening for incoming connections was successfull.
 *
 * Initializes the TCP server and starts listening for incoming connections on any address.<br>
 * Afterwards the IP-Address of the server is located and a signal (ServerSocket::newIP()) with the IP in QString format is emitted.<br>
 * If no IP was found, localhost is used as IP-Address.
 */
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

/**
 * @brief Signal to close the server.
 *
 * @return Returns true, if the server was closed successfully.
 */
bool ServerSocket::closeServer()
{
    this->close();
    qDebug() << "Closing server.\n";
    return true;
}

/**
 * @brief Send method for server.
 *
 * Sends data to all clients on method call.
 */
bool ServerSocket::send(QByteArray data)
{
    for(int i = 0; i < clientThreadList.size(); i++)
    {
        clientThreadList.at(i)->sendData(data);
    }
    return true;
}

/**
 * @brief Method for handling new connections from clients.
 *
 * This method is called every time a new client tries to connect to the server.<br>
 * Within the method a new TcpSocket object (ClientConnection class) is created for the connected client and put into an own thread.<br>
 * TODO: More documentation!
 */
void ServerSocket::incomingConnection(int socketDescriptor)
{
    // Increment counter for connected clients
    connectedClients++;

    // Create new client thread (ConnectedClient class), add client thread to thread list and start client thread
    ConnectedClient* newClient = new ConnectedClient(this, socketDescriptor, clientID);
    clientThreadList.append(newClient);

    // Connect signals and slots for communication between server- and client- object
    connect(newClient, SIGNAL(newData(QByteArray, unsigned int)), this, SLOT(handleNewData(QByteArray, unsigned int)));
    connect(newClient, SIGNAL(disconnected(unsigned int)), this, SLOT(handleClientDisconnect(unsigned int)));

    // Start the thread with low priority
    newClient->start(QThread::LowPriority);

    qDebug() << "New client connected to server with ID: " << clientID << "\n.";

    // Increment clientID afterwards
    clientID++;
}

bool ServerSocket::handleClientDisconnect(unsigned int clientID)
{
    qDebug() << "Client with ID " << clientID << " disconnected from server.\n";
    for(int i = 0; i < clientThreadList.length(); i++)
    {
        if(clientThreadList.at(i)->getClientID() == clientID)
        {
            //TODO: Causes chrash!!
            //clientThreadList.at(i)->quit();
            clientThreadList.at(i)->deleteLater();
            clientThreadList.removeAt(i);
            break;
        }
    }
    return true;
}

void ServerSocket::handleNewData(QByteArray data, unsigned int clientID)
{
    QString data_str(data);
    qDebug() << "New data from client with ID: " << clientID << "\n.";
    qDebug() << data_str << "\n.";
}
