/*
 * ClientSocket.cpp
 *
 *  Created on: 21.05.2015
 *      Author: Niklas
 */

#include "ClientSocket.h"

#include <QDebug>

ClientSocket::ClientSocket() : QObject()
{

}

ClientSocket::~ClientSocket()
{

}

void ClientSocket::init()
{
    qDebug() << "Init-Slot of client called!\n";
}
