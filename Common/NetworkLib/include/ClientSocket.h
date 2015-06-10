/*
 * ClientSocket.h
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

// Qt imports
#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>

namespace Network
{
    /**
     * @class ClientSocket ClientSocket.h "Network/ClientSocket.h".
     *
     * @brief Client Socket class.
     *
     * Instantiates two TCP Client Sockets (command and data) that can connect to servers.<br>
     * The static constants <i>cmdConnection</i> and <i>dataConnection</i> are used to define or determine the <i>connectionType</i> of the current method.<br>
     * This means if a function is called with one of the static constants as <i>connectionType</i>, the function can determine which type of connection to use.<br>
     * The class provides several signals and slots for connection and data handling:
     * <ul>
     *  <li>signals:</li>
     *      <ul>
     *          <li>connectedToCmdServer(): Emitted, when the command socket successfully connected to the server.</li>
     *          <li>connectedToDataServer(): Emitted, when the data socket successfully connected to the server.</li>
     *          <li>receivedCmd(): Emitted with ByteArray, when a new command is available.</li>
     *          <li>receivedData(): Emitted with ByteArray, when new data is available.</li>
     *          <li>lostConnection(): Emitted, when the connection to one of the servers (cmd or data) is lost.</li>
     *      </ul>
     *  <li>slots:</li>
     *      <ul>
     *          <li>connectToServer(): Tries to establish a connection to a server with the IP and two ports (command and data port) that are given as parameter in QString format.</li>
     *          <li>sendData(): Sends the data that is given as parameter to the server that is defined by <i>connectionType</i>.</li>
     *          <li>disconnectFromServer(): Disconnects the current connection to the server for both sockets.</li>
     *      </ul>
     *  </ul>
     */
    class ClientSocket : public QObject
    {
        Q_OBJECT
    public:
        /// Static constant for indicating a command connection.
        static const int cmdConnection = 0;
        /// Static constant for indicating a data connection.
        static const int dataConnection = 1;

        ClientSocket(QObject*);
        virtual ~ClientSocket();

    private:
        /// QTcpSocket for command connection.
        QTcpSocket* m_cmdSocket;
        /// QTcpSocket for data connection.
        QTcpSocket* m_dataSocket;

    signals:
        /**
         * @brief Signal that is emitted, when when the command socket successfully connected to its server.
         */
        void connectedToCmdServer();

        /**
         * @brief Signal that is emitted, when when the data socket successfully connected to its server.
         */
        void connectedToDataServer();

        /**
         * @brief Signal that is emitted, when a new command is available at the command socket.
         *
         * @param[out] data Command that was read from the socket in QByteArray format.
         */
        void receivedCmd(QByteArray data);

        /**
         * @brief Signal that is emitted, when new data is available at the data socket.
         *
         * @param[out] data Data that was read from the socket in QByteArray format.
         */
        void receivedData(QByteArray data);

        /**
         * @brief Signal that is emitted, when the connection was lost to one the server sockets.
         */
        void lostConnection();

    public slots:
        bool connectToServer(QString ipAddr_str, QString cmdPort_str, QString dataPort_str);
        void disconnectFromServer();
        int sendData(QByteArray data, int connectionType);

    private slots:
        void handleNewData();
    };
}
#endif /* CLIENTSOCKET_H_ */