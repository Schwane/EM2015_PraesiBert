/*
 * ServerSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "ServerSocket.h"

#include <QDebug>

ServerSocket::ServerSocket() : QObject()
{

}

ServerSocket::~ServerSocket()
{

}

void ServerSocket::init()
{
    qDebug() << "Init-Slot of server called!\n";
}
