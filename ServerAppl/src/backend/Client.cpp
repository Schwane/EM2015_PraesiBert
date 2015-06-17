/*
 * Client.cpp
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#include <src/backend/Client.h>

namespace ServerAppl
{
    unsigned int Client::clientCounter = 0;

    Client::Client()
    {
        identifier = clientCounter;
        clientCounter++;
        name = new QString("");
        lastTimestamp = new QTime();
    }

    Client::~Client()
    {
        delete(name);
        delete(lastTimestamp);
    }

    unsigned int Client::getIdentifier()
    {
        return identifier;
    }
} /* namespace ServerAppl */
