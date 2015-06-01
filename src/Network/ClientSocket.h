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

class ClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    ClientSocket(QObject*);
    virtual ~ClientSocket();

public slots:
    bool connectToServer(QString ipAddr_str, QString port_str);
    bool disconnectFromServer();
    bool handleConnect();
    bool handleNewData();
    int sendData(QByteArray data);

signals:
    void newData(QByteArray data);
};

#endif /* CLIENTSOCKET_H_ */