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

    Master::~Master()
    {
        // TODO Auto-generated destructor stub
    }

//    Message* handleReceivedMessage(QString commandName, Message* msg)
//    {
//        return NULL;
//    }

    void Master::onReceivedData(unsigned int receiverIdentifier)
    {
        /* TODO examine received data-packet, create presentation-data-object
         * and emit signal with pointer to packet
         */

        if(receiverIdentifier == getIdentifier())
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
