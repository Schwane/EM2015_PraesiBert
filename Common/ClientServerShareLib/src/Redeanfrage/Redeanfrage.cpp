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
Redeanfrage::prepare()
{
    //state changes are all the same
    // - set state
    // - emit state
    state = PREPARATION;
    emit stateChanged("PREPARED");
}

void
Redeanfrage::queue(QString clientId)
{
    state = QUEUED;
    this -> clientId = clientId; //set client id
    emit stateChanged("QUEUED");
}

void
Redeanfrage::queue()
{
    state = QUEUED;
    emit stateChanged("QUEUED");
}

void
Redeanfrage::accept()
{
    state = ACCEPTED;
    emit stateChanged("ACCEPTED");
}

void
Redeanfrage::reject()
{
    state = REJECTED;
    emit stateChanged("REJECTED");
}

void
Redeanfrage::finish()
{
    state = FINISHED;
    emit stateChanged("FINISHED");
}

Message*
Redeanfrage::packRedeanfrage()
{
    Message *msg = new Message(CMD_RANF_ASK, clientId, "master");
    //set state to queued by default
    state = QUEUED;
    msg->addParameter("clientId", clientId); //add client id
    emit stateChanged("QUEUED");
    return msg;
}

QString
Redeanfrage::getClientId()
{
    return clientId;
}

void
Redeanfrage::setClientId(QString clientId)
{
    this -> clientId = clientId;
}
