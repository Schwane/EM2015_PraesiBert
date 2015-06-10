/*
 * ConnectedClient.h
 *
 *  Created on: 28.05.2015
 *      Author: Niklas
 */

#ifndef CONNECTEDCLIENT_H_
#define CONNECTEDCLIENT_H_

// Qt includes
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QByteArray>

/**
 * @class ConnectedClient ConnectedClient.h "Network/ConnectedClient.h".
 *
 * @brief Class for clients connected to the server.
 *
 * Class for clients that connect to the Server Socket (ServerSocket class).<br>
 * All of the objects that are created from this class are stored in an individual thread that is started when a new connection is established.
 * The static constants <i>cmdConnection</i> and <i>dataConnection</i> are used to define or determine the <i>connectionType</i> of the current method.<br>
 * This means if a function is called with one of the static constants as <i>connectionType</i>, the function can determine which type of connection to use.<br>
 * The class provides several functions, signals and slots that are used to exchange data with a client:
 * <ul>
 *  <li>functions:</li>
 *      <ul>
 *          <li>setSocket(): Sets the socket that is given as parameter to either the command or data socket, depending on the connectionType.</li>
 *          <li>hasSocket(): Returns true, if the socket that was requested as parameter in connectionType is set up.</li>
 *          <li>sendData(): Sends ByteArray given as parameter to one of the client sockets, depending on the connectionType.</li>
 *          <li>disconnectFromServer(): Disconnects both sockets from the servers sockets.</li>
 *          <li>getClientID(): Returns the ID of the client.<li>
 *          <li>getPeerAddress(): Returns the peer address of the client.</li>
 *      </ul>
 *  <li>signals:</li>
 *      <ul>
 *          <li>newData(): Emitted with the clientID, data and connection type, when new data is available from one of the clients sockets.</li>
 *          <li>disconnected(): Emitted with the clientID, when the connection to one of the clients sockets is lost.</li>
 *          <li>finished(): Emitted, when the connection to the client is lost and the client socket can be destroyed.</li>
 *      </ul>
 *  <li>slots:</li>
 *      <ul>
 *          <li>process(): Calling this slot sets up the client socket. This slot is connected to the start signal of the thread that stores the object.</li>
 *      </ul>
 *  </ul>
 */
class ConnectedClient : public QObject
{
    Q_OBJECT
public:
    /// Static constant for indicating a command connection.
    static const int cmdConnection = 0;
    /// Static constant for indicating a data connection.
    static const int dataConnection = 1;

    ConnectedClient(uint clientID);
    virtual ~ConnectedClient();

    void setSocket(QTcpSocket* tcpSocket, int connectionType);
    bool hasSocketType(int connectionType);

    int sendData(QByteArray data, int connectionType);
    void disconnectFromServer();

    uint getClientID();
    QHostAddress getPeerAddress();

private:
    /// Integer value of the socket descriptor.
    uint m_socketDescriptor;
    /// ID of the client.
    uint m_clientID;
    /// Command Socket of the client.
    QTcpSocket* m_cmdSocket;
    /// Data Socket of the client.
    QTcpSocket* m_dataSocket;
    /// Boolean that stores, whether the command socket has been set up.
    bool hasCmdSocket;
    /// Boolean that stores, whether the data socket has been set up.
    bool hasDataSocket;

signals:
    /**
      * @brief Signal that is emitted, when new data is available from a socket of the client.
      *
      * @param[out] data Data in QByteArray format that is send out.
      * @param[out] clientID Own ID of the client that sends the data.
      * @param[out] connectionType Type of connection, defined by one of the static constants of this class.
      */
     void newData(QByteArray data, uint clientID, int connectionType);

    /**
     * @brief Signal that is emitted, when the client was disconnected.
     *
     * @param[out] clientID ID of the client that lost the connection.
     */
    void disconnected(uint clientID);

    /**
     * @brief Signal that is emitted, when the client is finished and ready for deletion.
     */
    void finished();

public slots:
    void process();

private slots:
    void handleCmdRead();
    void handleDataRead();
    void handleDisconnect();
};

#endif /* CONNECTEDCLIENT_H_ */
