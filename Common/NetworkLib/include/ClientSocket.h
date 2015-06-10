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
#include <QTcpSocket>
#include <QString>
#include <QByteArray>

/**
 * @class ClientSocket ClientSocket.h "Network/ClientSocket.h".
 *
 * @brief Client Socket class.
 *
 * Instantiates a TCP Client Socket that can connect to servers.<br>
 * The class inherits from the <i>QTcpSocket</i>-class.
 * Besides the methods of <i>QTcpSocket</i> it provides several signals and slots connection and data handling:
 * <ul>
 *  <li>signals:</li>
 *      <ul>
 *          <li>newData(): Emitted with ByteArray, when new data is available.</li>
 *          <li>lostConnection(): Emitted, when connection to the server is lost.</li>
 *      </ul>
 *  <li>slots:</li>
 *      <ul>
 *          <li>connectToServer(): Tries to establish a connection to a server with the IP and port that is given as parameter.</li>
 *          <li>disconnectFromServer(): Disconnects the current connection to the server.</li>
 *          <li>sendData(): Sends the data to the server that is given as parameter.</li>
 *      </ul>
 *  </ul>
 */
class ClientSocket : public QObject
{
    Q_OBJECT
public:
    static const int cmdConnection = 0;
    static const int dataConnection = 1;

    ClientSocket(QObject*);
    virtual ~ClientSocket();

private:
    QTcpSocket* m_cmdSocket;
    QTcpSocket* m_dataSocket;

signals:
    /**
     * @brief Signal that is emitted, when new data is available at the client socket.
     *
     * @param[out] data Data that was read from the socket.
     */
    void receivedCmd(QByteArray data);

    void receivedData(QByteArray data);

    void connectedToCmdServer();

    void connectedToDataServer();

    /**
     * @brief Signal that is emitted, when the connection was lost to the server.
     */
    void lostConnection();

public slots:
    bool connectToServer(QString ipAddr_str, QString cmdPort_str, QString dataPort_str);
    int sendData(QByteArray data, int connectionType);
    void disconnectFromServer();

private slots:
    void handleNewData();
};

#endif /* CLIENTSOCKET_H_ */
