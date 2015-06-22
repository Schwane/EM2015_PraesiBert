/*
 * Master.cpp
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#include <src/backend/Master.h>

namespace ServerAppl
{
    Master::Master()
    {
        acceptSlides = FALSE;

    }

    Master::Master(UnspecifiedClient * priorClientObject, QString nonce1 )
    {
        this->priorClientObject = priorClientObject;
        this->server = priorClientObject->getServer();
        this->clientId = priorClientObject->getClientId();
        this->name = priorClientObject->getName();
        this->lastTimestamp = priorClientObject->getLastTimestamp();
        this->nonce.part_1 = nonce1;
        acceptSlides =  FALSE;
    }

    Master::~Master()
    {
        // TODO Auto-generated destructor stub
    }

    bool Master::createMaster(UnspecifiedClient * client, Master* master, QString nonce1)
    {
        master = new Master(client, nonce1);

        return true;
    }

    Message* Master::handleReceivedMessage(QString commandName, Message* msg)
    {
        Message * responseMessage;

        if(IS_COMMAND(commandName, "PROOF_RESPONSE"))
        {
            responseMessage = handleProofResponse(commandName, msg);
        }
        else if(IS_COMMAND(commandName, "LOGIN_RESPONSE"))
        {
            responseMessage = handleLoginResponse(commandName, msg);
        }
        else
        {

        }

        return responseMessage;
    }

    Message* Master::handleProofResponse(QString commandName, Message* msg)
    {
        Message * responseMessage;

        if(IS_COMMAND(commandName, "PROOF_RESPONSE"))
        {
            responseMessage = new Message(QString("login"), QString("server"), QString("client"));

        }

        return responseMessage;
    }

    Message* Master::handleLoginResponse(QString commandName, Message* msg)
    {
        return NULL;
    }

    void Master::onReceivedData(unsigned int receiverIdentifier)
    {
        /* TODO examine received data-packet, create presentation-data-object
         * and emit signal with pointer to packet
         */

        if(receiverIdentifier == getClientId())
        {
            /* received slides */
            acceptSlides = FALSE;

            //TODO create presentation-data-object

            emit receivedSlides();
        }

    }

    void Master::onTransmitSlidesResponse(bool accepted)
    {
        acceptSlides = accepted;
    }

} /* namespace ServerAppl */
