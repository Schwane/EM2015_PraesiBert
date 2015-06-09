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
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>

// Other includes
#include "ConnectedClient.h"

/**
 * @class ServerSocket ServerSocket.h "Network/ServerSocket.h".
 *
 * @brief Server Socket class.
 *
 * Instantiates a TCP Server Socket that clients can connect to for communication.<br>
 *
 * The class inherits from the <i>QTcpServer</i>-class.<br>
 * Besides the methods of <i>QTcpServer</i> it provides several signals and slots connection and data handling:
 * <ul>
 *  <li>signals:</li>
 *      <ul>
 *          <li>newIP(): Emitted with current IP, when the server is set up.</li>
 *          <li>clientDisconnect(): Emitted with clientID, when connection to a client is lost.</li>
 *          <li>stoppedServer(): Emitted, when server is stopped.</li>
 *          <li>receiveFromClient(): Emitted with ByteArray and clientID, when a message is received from a client.</li>
 *      </ul>
 *  <li>slots:</li>
 *      <ul>
 *          <li>beginListening(): Calling this slot makes the server start to listen for incoming connections of the port that is given as parameter.</li>
 *          <li>closeServer(): Shuts down the server.</li>
 *          <li>sendToAll(): Sends the ByteArray that is given as parameter to all of it's clients.</li>
 *      </ul>
 *  </ul>
 *
 * The ServerSocket manages all of the connected clients in a list (m_clientList) with a specific ID.
 * For any client that establishes a connection to the server, an object of the <i>ConnectedClient</i>-class is created, pushed to an own thread and stored in m_clientList.<br>
 * The <i>ConnectedClient</i>-class contains a QTcpSocket that the server can use to communicate with it's clients.
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
    /// Variable that stores the amount of connected clients.
    uint m_connectedClients;
    /// ID that is given to a new connected client.
    uint m_clientID;

private slots:
    void handleNewConnection();
    void handleClientDisconnect(uint clientID);
    void handleNewRead(QByteArray data, uint clientID, int connectionType);

public slots:
    bool beginListening(QString cmdPort_str, QString dataPort_str);
    void closeServer();
    void sendToAll(QByteArray data, int connectionType);
    int sendToID(QByteArray data, uint clientID, int connectionType);

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
     * @brief Signal that is emitted, when data was received from a client.
     *
     * @param[out] data Data that was received from the client.
     * @param[out] clientID ID of the client that sent the data.
     */
    void receivedCmdFromClient(QByteArray data, uint clientID);

    //TODO: Docu!
    void receivedDataFromClient(QByteArray data, uint clientID);
};

#endif /* SERVERSOCKET_H_ */
