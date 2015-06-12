/*
 * ServerSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "include/ServerSocket.h"

// QT includes
#include <QDebug>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QList>
#include <QThread>

namespace Network
{
    /**
     * @brief Constructor of the ServerSocket class.
     *
     * @param[in] parent Parent QObject that creates the server socket object.
     *
     * The constructor initializes <i>m_clientID</i> (ID that is given to client) with its initial value <i>0</i>.<br>
     * Also initializes both types of server sockets with <i>parent</i> as parameter.
     */
    ServerSocket::ServerSocket(QObject* parent)
            : QObject(parent)
            , m_clientID(0)
    {
        m_cmdServer = new QTcpServer(parent);
        m_dataServer = new QTcpServer(parent);
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
     * @param[in] cmdPort_str Listening port for incoming command connections in QString format.
     * @param[in] dataPort_str Listening port for incoming data connections in QString format.
     *
     * @return Returns true, if the listening for incoming connections started successfully.
     *
     * First the IP-Address of the server is located and the signal <i>newIP()</i> with the IP in QString format is emitted.<br>
     * If no IP was found, localhost is used as IP-Address.<br>
     * Afterwards initializes the TCP server sockets and starts listening for incoming connections on any address.<br>
     * Also connects the signals <i>newConnection</i> of the server sockets with the handler slot <i>handleNewConnection()</i>.
     */
    bool ServerSocket::beginListening(QString cmdPort_str, QString dataPort_str)
    {
        // Define variable for IP-Address
        QString ipAddress_str;

        // Fetch available IP-Addresses
        QList<QHostAddress> ipList = QNetworkInterface::allAddresses();

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

        // Start listening for incoming connections on both sockets
        if(!m_cmdServer->listen(QHostAddress::Any, cmdPort_str.toInt(0, 10)))
        {
            // Close socket and return Error-State, if listening failed
            qDebug() << "Could not start the Command-Server. Closing sockets.\n";
            m_cmdServer->close();
            return false;
        }

        if(!m_dataServer->listen(QHostAddress::Any, dataPort_str.toInt(0, 10)))
        {
            // Close socket and return Error-State, if listening failed
            qDebug() << "Could not start the Data-Server. Closing sockets.\n";
            m_cmdServer->close();
            return false;
        }

        connect(m_cmdServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
        connect(m_dataServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));

        qDebug() << "Listening for incoming command connections on Port " << cmdPort_str << ".\n";
        qDebug() << "Listening for incoming data connections on Port " << dataPort_str << ".\n";
        qDebug() << "IP-Address is: " << ipAddress_str << ".\n";
        return true;
    }

    /**
     * @brief Signal to close the server.
     *
     * Disconnects from all of the clients in <i>m_clientList</i> and closes both servers.
     * Emits the signal <i>stoppedServer</i> afterwards.
     */
    void ServerSocket::closeServer()
    {
        qDebug() << "Closing Command- and Data-Server.\n";

        // Close connection to each Client
        for(int i = 0; i < m_clientList.length(); i++)
        {
            m_clientList.at(i)->disconnectFromServer();
        }

        m_cmdServer->close();
        m_dataServer->close();
        emit stoppedServer();
    }

    /**
     * @brief Method for sending a command to all clients.
     *
     * @param[in] data Command that is send to the clients.
     *
     * Sends a command to all clients.
     */
    void ServerSocket::sendCmdToAll(QByteArray data)
    {
        for(int i = 0; i < m_clientList.size(); i++)
        {
            m_clientList.at(i)->sendData(data, ConnectedClient::cmdConnection);
        }
    }

    /**
     * @brief Method for sending data to all clients.
     *
     * @param[in] data Data that is send to the clients.
     *
     * Sends data to all clients.
     */
    void ServerSocket::sendDataToAll(QByteArray data)
    {
        for(int i = 0; i < m_clientList.size(); i++)
        {
            m_clientList.at(i)->sendData(data, ConnectedClient::dataConnection);
        }
    }

    /**
     * @brief Method for sending a command to a client with specified ID.
     *
     * @param[in] data Command that is send to the clients.
     * @param[in] clientID ID of the client that the command is send to.
     *
     * Sends a command to the client with the specified ID.
     */
    int ServerSocket::sendCmdToID(QByteArray data, uint clientID)
    {
        for(int i = 0; i < m_clientList.size(); i++)
        {
            if(m_clientList.at(i)->getClientID() == clientID)
            {
                // Send data and return amount of sent data in bytes
                return m_clientList.at(i)->sendData(data, ConnectedClient::cmdConnection);
            }
        }
        return 0;
    }

    /**
     * @brief Method for sending data to a client with specified ID.
     *
     * @param[in] data Data that is send to the clients.
     * @param[in] clientID ID of the client that the data is send to.
     *
     * Sends data to the client with the specified ID.
     */
    int ServerSocket::sendDataToID(QByteArray data, uint clientID)
    {
        for(int i = 0; i < m_clientList.size(); i++)
        {
            if(m_clientList.at(i)->getClientID() == clientID)
            {
                // Send data and return amount of sent data in bytes
                return m_clientList.at(i)->sendData(data, ConnectedClient::dataConnection);
            }
        }
        return 0;
    }

    /**
     * @brief Method for handling new connections from clients.
     *
     * This method is called every time a new client tries to connect to one of the server sockets.<br>
     * Within the method a new ClientConnection class object (with two QTcpSockets) is created for the connected client and put into an own thread.<br>
     *
     * The new connection is set up the following way:
     * <ol>
     *  <li>The sender which called this slot, is determined.</li>
     *  <li>Depending on the sender, a new QTcpSocket object is read from one of the server sockets.</li>
     *  <li>A loop checks if the peer address of the new socket is already in the list of clients.<li>
     *       <ul>
     *          <li>If so, the socket is added to this client and the function returns.</li>
     *          <li>If not, a new ConnectedClient object (newClient) is created and the socket is added to that client.</li>
     *       </ul>
     *  <li>A new thread is created and the newClient is moved to the thread.</li>
     *  <li>Signals and slots of the thread and newClient are connected with each other.</li>
     *  <li>newClient is appended to the list of connected clients (m_clientList).</li>
     *  <li>Signals and slots of the server and newClient are connected with each other.</li>
     *  <li>The client's thread is started.</li>
     *  <li>The clientID is incremented for the next connection.</li>
     * </ol>
     */
    void ServerSocket::handleNewConnection()
    {
        QTcpServer* signalSender;
        signalSender = qobject_cast<QTcpServer*>(sender());

        QTcpSocket* newSocket;

        if(signalSender == m_cmdServer)
        {
            newSocket = m_cmdServer->nextPendingConnection();
        }
        if(signalSender == m_dataServer)
        {
            newSocket = m_dataServer->nextPendingConnection();
        }

        /*
         * Check, if a Client with the same peer address is already in the clientList.
         * If so, just add the new socket type to the client.
         * Otherwise a new ConnectedClient Object will be created later.
         */
        for(int i = 0; i < m_clientList.length(); i++)
        {
            if(m_clientList.at(i)->getPeerAddress() == newSocket->peerAddress())
            {
                qDebug() << "Found existing Client with ID " << m_clientList.at(i)->getClientID() << " and IP-Address " << m_clientList.at(i)->getPeerAddress().toString() << ".\n";
                if(signalSender == m_cmdServer)
                {
                    m_clientList.at(i)->setSocket(newSocket, ConnectedClient::cmdConnection);
                    qDebug() << "Added new Command-Socket to Client with ID " << m_clientList.at(i)->getClientID() << ".\n";
                }
                if(signalSender == m_dataServer)
                {
                    m_clientList.at(i)->setSocket(newSocket, ConnectedClient::dataConnection);
                    qDebug() << "Added new Data-Socket to Client with ID " << m_clientList.at(i)->getClientID() << ".\n";
                }
                return;
            }
        }

        // Create client thread and a new client (ConnectedClient class) and move it to the client thread
        // Do not give a parent to newClient, so it can be moved to a thread
        ConnectedClient* newClient;
        newClient = new ConnectedClient(m_clientID);

        if(signalSender == m_cmdServer)
        {
            newClient->setSocket(newSocket, ConnectedClient::cmdConnection);
            qDebug() << "Created new client plus Command-Socket with ID " << m_clientID << ".\n";
        }
        if(signalSender == m_dataServer)
        {
            newClient->setSocket(newSocket, ConnectedClient::dataConnection);
            qDebug() << "Created new client plus Data-Socket with ID " << m_clientID << ".\n";
        }

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
        connect(newClient, SIGNAL(newData(QByteArray, uint, int)), this, SLOT(handleNewRead(QByteArray, uint, int))); // Process data when it is available from a client
        connect(newClient, SIGNAL(disconnected(uint)), this, SLOT(handleClientDisconnect(uint))); // Handle a disconnect of a client

        // Start the thread with low priority
        thread->start(QThread::LowPriority);

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

        emit clientDisconnect(clientID);
    }

    /**
     * @brief Handler for new data of a client.
     *
     * @param[in] data Data that was send from a client in QByteArray format.
     * @param[in] clientID ID of the client that send the data.
     * @param[in] connectionType Type of connection that the data is available at.
     *
     * This method handles the data that was send by a client.<br>
     * It determines which type of data is available from a client with <i>connectionType</i>.<br>
     * It emits the signal <i>receivedCmdFromClient()</i> with the data in QByteArray format and the client ID as parameters, if the new data was a command.
     * It emits the signal <i>receivedDataFromClient()</i> with the data in QByteArray format and the client ID as parameters, if the new data was actually data.
     */
    void ServerSocket::handleNewRead(QByteArray data, uint clientID, int connectionType)
    {
        QString data_str(data);
        if(connectionType == ConnectedClient::cmdConnection)
        {
            qDebug() << "New command read from client with ID: " << clientID << ".\n";
            qDebug() << data_str << "\n.";
            emit receivedCmdFromClient(data, clientID);
            return;
        }
        if(connectionType == ConnectedClient::dataConnection)
        {
            qDebug() << "New data read from client with ID: " << clientID << ".\n";
            qDebug() << data_str << ".\n";
            emit receivedDataFromClient(data, clientID);
            return;
        }
    }
}
