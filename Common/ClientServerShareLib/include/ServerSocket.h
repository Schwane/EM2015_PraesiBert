/*
 * ServerSocket.h
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

// Qt includes
#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QList>

// Other includes
#include "ConnectedClient.h"

namespace Network
{
    /**
     * @class ServerSocket ServerSocket.h "Network/ServerSocket.h"
     *
     * @brief ServerSocket class.
     *
     * Instantiates two TCP Server Sockets (command and data) that clients can connect to for communication.<br>
     * The class provides several signals and slots for connection and data handling:
     * <ul>
     *  <li>signals:</li>
     *      <ul>
     *          <li>newIP(): Emitted with current IP, when the server is set up.</li>
     *          <li>clientDisconnect(): Emitted with clientID, when connection to a client is lost.</li>
     *          <li>stoppedServer(): Emitted, when server is stopped.</li>
     *          <li>receivedCmdFromClient(): Emitted with data and clientID, when a command was received from a client.</li>
     *          <li>receivedDataFromClient(): Emitted with data and clientID, when data was received from a client.</li>
     *      </ul>
     *  <li>slots:</li>
     *      <ul>
     *          <li>beginListening(): Calling this slot makes the server start to listen for incoming connections (command and data) of the ports that are given as parameter.</li>
     *          <li>closeServer(): Shuts down the server.</li>
     *          <li>sendCmdToAll(): Sends the command that is given as parameter to all of its clients command sockets.</li>
     *          <li>sendDataToAll(): Sends the data that is given as parameter to all of its clients data sockets.</li>
     *          <li>sendCmdToID(): Sends the command that is given as parameter to the client with the specified ID.</li>
     *          <li>sendDataToID(): Sends the data that is given as parameter to the client with the specified ID.</li>
     *          <li>sendCmdToMultClients(): Sends the command that is given as parameter to the IDs of clients that are given in a QList as parameter.</li>
     *          <li>sendCmdToMultClients(): Sends the data that is given as parameter to the IDs of clients that are given in a QList as parameter.</li>
     *          <li>disconnectFromClient(): Closes the connection to the client whose ID is given as parameter.</li>
     *      </ul>
     *  </ul>
     * The ServerSocket manages all of the connected clients in a list (m_clientList) with a specific ID.
     * For each client that establishes a connection to the server, an object of the <i>ConnectedClient</i>-class is created, pushed to an own thread and stored in m_clientList.<br>
     * The <i>ConnectedClient</i>-class contains two QTcpSockets that the server can use to communicate with it's clients.
     */
    class ServerSocket : public QObject
    {
        Q_OBJECT
    public:
        ServerSocket(QObject* parent);
        virtual ~ServerSocket();

    private:
        /// QTcpServer for command connections
        QTcpServer* m_cmdServer;
        /// QTcpServer for data connections
        QTcpServer* m_dataServer;
        /// QList that stores the connected clients.
        QList<ConnectedClient*> m_clientList;
        /// ID that is given to a new connected client.
        uint m_clientID;

    signals:
        /**
         * @brief Signal that is emitted, when the server is set up correctly and a correct IP was found.
         *
         * @param[out] newIP IP-Address in QString format that was found.
         */
        void newIP(QString newIP);

        /**
         * @brief Signal that is emitted, when the connection to a client was lost.
         *
         * @param[out] clientID ID of the client that the connection was lost to.
         */
        void clientDisconnect(uint clientID);

        /**
         * @brief Signal that is emitted, when the server was stopped.
         */
        void stoppedServer();

        /**
         * @brief Signal that is emitted, when a new command was received from a client.
         *
         * @param[out] data Command that was received from the client.
         * @param[out] clientID ID of the client that sent the command.
         */
        void receivedCmdFromClient(QByteArray data, uint clientID);

        /**
         * @brief Signal that is emitted, when new data was received from a client.
         *
         * @param[out] data Data that was received from the client.
         * @param[out] clientID ID of the client that sent the data.
         */
        void receivedDataFromClient(QByteArray data, uint clientID);

        /**
         * @brief Signal that is emitted, when a new client connected to the server.
         *
         * @param[out] clientID ID of the client that connected to the server.
         *
         * This Signal is only emitted, when <b>both</b> types of sockets (data and command) successfully connected to the server.
         */
        void newClient(uint clientID);

    public slots:
        bool beginListening(QString cmdPort_str, QString dataPort_str);
        void closeServer();
        bool disconnectFromClient(uint clientID);
        void sendCmdToAll(QByteArray data);
        void sendDataToAll(QByteArray data);
        void sendCmdToMultClients(QByteArray data, QList<uint> clientIDs);
        void sendDataToMultClients(QByteArray data, QList<uint> clientIDs);
        int sendCmdToID(QByteArray data, uint clientID);
        int sendDataToID(QByteArray data, uint clientID);

    private slots:
        void handleNewConnection();
        void handleClientDisconnect(uint clientID);
    };
}
#endif /* SERVERSOCKET_H_ */
