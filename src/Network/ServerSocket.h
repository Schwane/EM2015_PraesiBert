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

/**
 * @brief ConnectionServer class
 *
 * Instantiates a server socket that clients can connect to for non-massive data communication.
 */
class ServerSocket : public QTcpServer
{
    Q_OBJECT
public:
    ServerSocket(QObject* parent);
    virtual ~ServerSocket();

private:
    QTcpSocket* aTcpConnection;

public slots:
    bool beginListening(QString port_str);
    bool closeServer();
    bool send();
    bool handleClientConnection();
    bool handleClientDisconnect();

signals:
    void newIP(QString newIP);
    void clientDisconnect();
    void stoppedServer();
    //void receive();
};

#endif /* SERVERSOCKET_H_ */
