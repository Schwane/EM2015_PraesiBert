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
 * Instantiates a server socket that clients can connect to for communication.
 */
class ServerSocket : public QTcpServer
{
    Q_OBJECT
public:
    ServerSocket(QObject* parent);
    virtual ~ServerSocket();

private:
    ConnectedClient* newClient;
    QList<ConnectedClient*> clientList;
    unsigned int connectedClients;
    unsigned int clientID;

public slots:
    bool beginListening(QString port_str);
    bool closeServer();
    bool send(QByteArray data);
    bool handleClientDisconnect(unsigned int clientID);
    void incomingConnection(int socketDescriptor);
    void handleNewData(QByteArray data, unsigned int clientID);

signals:
    void newIP(QString newIP);
    void clientDisconnect();
    void stoppedServer();
    void receiveFromClient(uint clientID, QByteArray* data);
};

#endif /* SERVERSOCKET_H_ */
