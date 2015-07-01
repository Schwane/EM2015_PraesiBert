/*
 * Redeanfrage.cpp
 *
 *  Created on: 01.07.2015
 *      Author: JCB
 */

#include "include/Redeanfrage.hpp"

Redeanfrage::Redeanfrage()
{
    state = PREPARATION;
    clientId = "undefined";
}

Redeanfrage::Redeanfrage(QString clientId)
{
    state = PREPARATION;
    this -> clientId = clientId;
}

Redeanfrage::~Redeanfrage()
{
    // TODO Auto-generated destructor stub
}

void
Redeanfrage::queue(QString clientId)
{
    state = QUEUED;
    this -> clientId = clientId;
    emit queued();
}

void
Redeanfrage::accept()
{
    state = ACCEPTED;
    emit accepted();
}

void
Redeanfrage::reject()
{
    state = REJECTED;
    emit rejected();
}
Message*
Redeanfrage::packRedeanfrage()
{
    Message *msg = new Message(CMD_RANF_ASK, clientId, "master");
    state = QUEUED;
    emit queued();
    return msg;
}
