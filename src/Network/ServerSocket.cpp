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
#include <QThread>

/**
 * @brief Constructor of the ServerSocket class.
 *
 * @param[in] parent Parent QObject that creates the server socket object.
 *
 * The constructor initializes the super class and the variables <i>m_connectedClients</i>(amount of connected clients) and <i>m_clientID</i>(ID that is given to client) with their initial value <i>0</i>.
 */
ServerSocket::ServerSocket(QObject* parent)
        : QTcpServer(parent)
        , m_connectedClients(0)
        , m_clientID(0)
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
 * @return Returns true, if the listening for incoming connections started successfully.
 *
 * Initializes the TCP server and starts listening for incoming connections on any address.<br>
 * Afterwards the IP-Address of the server is located and the signal <i>newIP()</i> with the IP in QString format is emitted.<br>
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
 *
 * Emits the signal <i>stoppedServer</i> after it closed the server.
 */
void ServerSocket::closeServer()
{
    qDebug() << "Closing server.\n";

    // Close connection to each Client
    for(int i = 0; i < m_clientList.length(); i++)
    {
        m_clientList.at(i)->disconnectFromServer();
    }

    this->close();
    emit stoppedServer();
}

/**
 * @brief Method for sending data to all clients.
 *
 * @param[in] data Data that is send to the clients.
 *
 * Sends data to all clients on method call.
 */
void ServerSocket::sendToAll(QByteArray data)
{
    for(int i = 0; i < m_clientList.size(); i++)
    {
        m_clientList.at(i)->sendData(data);
    }
}

/**
 * @brief Method for handling new connections from clients.
 *
 * This method is called by the super class every time a new client tries to connect to the server.<br>
 * Within the method a new ClientConnection class object (with a QTcpSocket) is created for the connected client and put into an own thread.<br>
 *
 * The new connection is set up the following way:
 * <ol>
 *  <li>The value for the amount of connected clients is incremented.</li>
 *  <li>A new ConnectedClient object (newClient) is created. The constructor gets the socketDescriptor of this function call and the clientID as parameter.</li>
 *  <li>A new thread is created and newClient is moved to the thread.</li>
 *  <li>Signals and slots of the thread and newClient are connected with each other.</li>
 *  <li>newClient is appended to the list of connected clients (m_clientList).</li>
 *  <li>Signals and slots of the server and newClient are connected with each other.</li>
 *  <li>The client's thread is started.</li>
 *  <li>The clientID is incremented for the next connection.</li>
 * </ol>
 */
void ServerSocket::incomingConnection(int socketDescriptor)
{
    // Increment counter for connected clients
    m_connectedClients++;

    // Create client thread and a new client (ConnectedClient class) and move it to the client thread
    // Do not give a parent to newClient, so it can be moved to a thread
    ConnectedClient* newClient = new ConnectedClient(socketDescriptor, m_clientID);
    QThread* thread = new QThread(this);
    newClient->moveToThread(thread);

    // Connect signals and slots for thread start and stop
    connect(thread, SIGNAL(started()), newClient, SLOT(process())); // Call process() on thread start
    connect(newClient, SIGNAL(finished()), thread, SLOT(quit())); // Quit thread when client socket is finished
    connect(newClient, SIGNAL(finished()), newClient, SLOT(deleteLater())); // Delete client socket when it's finished
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater())); // Delete thread when it is finished

    // Append client thread to thread list.
    m_clientList.append(newClient);

    // Connect signals and slots for communication between server- and client- object
    connect(newClient, SIGNAL(newData(QByteArray, uint int)), this, SLOT(handleNewData(QByteArray, uint))); // Process data when it is available from a client
    connect(newClient, SIGNAL(disconnected(uint)), this, SLOT(handleClientDisconnect(uint))); // Handle a disconnect of a client

    // Start the thread with low priority
    thread->start(QThread::LowPriority);

    qDebug() << "New client connected to server with ID: " << m_clientID << ".\n";

    // Increment clientID afterwards
    m_clientID++;
}


/**
 * @brief Handler for client disconnects.
 *
 * @param[in] clientID ID of the client that was disconnected from the server.
 *
 * The method removes the client with the ID that is given as parameter and decrements the amount of connected clients.<br>
 * It then emits the signal <i>clientDisconnect()</i> with the ID of the client that was disconnected.
 */
void ServerSocket::handleClientDisconnect(uint clientID)
{
    qDebug() << "Client with ID " << clientID << " disconnected from server.\n";
    // Search for client in client list with given ID
    for(int i = 0; i < m_clientList.length(); i++)
    {
        if(m_clientList.at(i)->getClientID() == clientID)
        {
            // Remove client with the correct ID and leave loop
            m_clientList.removeAt(i);
            break;
        }
    }

    // Remove amount of connected clients
    m_connectedClients--;
    emit clientDisconnect(clientID);
}

/**
 * @brief Handler for new data of a client.
 *
 * @param[in] data Data that was send from a client in QByteArray format.
 * @param[in] clientID ID of the client that send the data.
 *
 * This method handles the data that was send by a client.<br>
 * It emits the signal <i>receivedFromClient()</i> with the data in QByteArray format and the client ID as parameters.
 */
void ServerSocket::handleNewData(QByteArray data, uint clientID)
{
    QString data_str(data);
    qDebug() << "New data from client with ID: " << clientID << "\n.";
    qDebug() << data_str << "\n.";
    emit receivedFromClient(data, clientID);
}
