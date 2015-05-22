/*
 * ServerSocket.h
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <QObject>

class ServerSocket : public QObject
{
    Q_OBJECT
public:
    ServerSocket();
    virtual ~ServerSocket();

public slots:
    void init();
};

#endif /* SERVERSOCKET_H_ */
