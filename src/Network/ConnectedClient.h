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
#include <QThread>

class ConnectedClient : public QThread
{
    Q_OBJECT
public:
    ConnectedClient(QObject* parent, int socketDescriptor, unsigned int clientID);
    virtual ~ConnectedClient();

    unsigned int getClientID();

private:
    unsigned int socketDescriptor;
    unsigned int clientID;
    QTcpSocket* tcpSocket;

private slots:
    void handleDataRead();
    void handleDisconnect();

public slots:
    int sendData(QByteArray data);

signals:
    void disconnected(unsigned int clientID);
    void newData(QByteArray data, unsigned int clientID);
};

#endif /* CONNECTEDCLIENT_H_ */
