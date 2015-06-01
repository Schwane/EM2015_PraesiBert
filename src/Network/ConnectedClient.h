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
#include <QByteArray>

/**
 * @class ConnectedClient ConnectedClient.h "Network/ConnectedClient.h".
 *
 * @brief Class for clients connected to server.
 *
 * Class for clients that connect to the Server Socket (ServerSocket class).<br>
 *
 * The class provides several signals and slots that can be used to exchange data with a client:
 * <ul>
 *  <li>signals:</li>
 *      <ul>
 *          <li>finished(): Emitted, when the connection to the client is lost and the client socket can be destroyed.</li>
 *          <li>disconnected(): Emitted with the clientID, when the connection to the client is lost.</li>
 *          <li>newData(): Emitted with the clientID and ByteArray, when new data is available from the client.</li>
 *      </ul>
 *  <li>slots:</li>
 *      <ul>
 *          <li>process(): Calling this method sets up the client socket. This method is connected to the start signal of the thread that stores the object.</li>
 *          <li>sendData(): Sends the given ByteArray to the client.</li>
 *      </ul>
 *  </ul>
 */
class ConnectedClient : public QObject
{
    Q_OBJECT
public:
    ConnectedClient(int socketDescriptor, uint clientID);
    virtual ~ConnectedClient();

    uint getClientID();

private:
    /// Integer value of the socket descriptor.
    uint m_socketDescriptor;
    /// ID of the client.
    uint m_clientID;
    /// Socket of the client.
    QTcpSocket* m_tcpSocket;

private slots:
    void handleDataRead();
    void handleDisconnect();

public slots:
    void process();
    int sendData(QByteArray data);

signals:
    /**
     * @brief Signal that is emitted, when the client is finished and ready for deletion.
     */
    void finished();

    /**
     * @brief Signal that is emitted, when the client was disconnected.
     *
     * @param[in] clientID ID of the client that lost the connection.
     */
    void disconnected(uint clientID);

    /**
     * @brief Signal that is emitted, when new data is available from a client.
     *
     * @param[in] data Data in QByteArray format that is send out.
     * @param[in] clientID Own ID of the client that sends the data.
     */
    void newData(QByteArray data, uint clientID);
};

#endif /* CONNECTEDCLIENT_H_ */
