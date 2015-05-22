/*
 * ClientSocket.h
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <QObject>

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    ClientSocket();
    virtual ~ClientSocket();

public slots:
    void init();
};

#endif /* CLIENTSOCKET_H_ */
