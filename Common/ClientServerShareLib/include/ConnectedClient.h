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
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QByteArray>

namespace Network
{
    /**
     * @class ConnectedClient ConnectedClient.h "Network/ConnectedClient.h".
     *
     * @brief Class for clients connected to the server.
     *
     * Class for clients that connect to the Server Socket (ServerSocket class).<br>
     * It uses 64 bit integers for determining the length of sent and received data.<br>
     * All of the objects that are created from this class are stored in an individual thread that is started when a new connection is established.<br>
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

        ConnectedClient(uint clientID);
        virtual ~ConnectedClient();

        void setCmdSocket(QTcpSocket* tcpSocket);
        void setDataSocket(QTcpSocket* tcpSocket);
        bool hasCmdSocket();
        bool hasDataSocket();

        int sendCmd(QByteArray data);
        int sendData(QByteArray data);
        void disconnectFromServer();

        uint getClientID();
        QHostAddress getPeerAddress();

    private:
        /// ID of the client.
        uint m_clientID;
        /// Command Socket of the client.
        QTcpSocket* m_cmdSocket;
        /// Data Socket of the client.
        QTcpSocket* m_dataSocket;
        /// Boolean that stores, whether the command socket has been set up.
        bool m_hasCmdSocket;
        /// Boolean that stores, whether the data socket has been set up.
        bool m_hasDataSocket;
        /// Variable for storing the size of the received blocks of commands.
        qint64 m_next_block_size_cmd;
        /// Variable for storing the size of the received blocks of data.
        qint64 m_next_block_size_data;
        /// Buffer for commands received at the command socket.
        QByteArray m_bufferCmd;
        /// Buffer for data received at the data socket.
        QByteArray m_bufferData;
        /// Will be used to avoid double-handling of disconnect.
        bool disconnectHandled;

    signals:
         /**
         * @brief Signal that is emitted, whe an newcommanda is available fromthe commanda socket of the client.
         *
         * @param[out] data Data in QByteArray format that is send out.
         * @param[out] clientID Own ID of the client that sends the data.
         */
        void newCmd(QByteArray data, uint clientID);

        /**
        * @brief Signal that is emitted, when new data is available from the data socket of the client.
        *
        * @param[out] data Data in QByteArray format that is send out.
        * @param[out] clientID Own ID of the client that sends the data.
        */
        void newData(QByteArray data, uint clientID);

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
}
#endif /* CONNECTEDCLIENT_H_ */
