/*
 * MessageHandlerInterface.h
 *
 *  Created on: 16.06.2015
 *      Author: sebastian
 */

#ifndef MESSAGEHANDLERINTERFACE_H_
#define MESSAGEHANDLERINTERFACE_H_

#include <QString>

#include <Message.hpp>

namespace ServerAppl
{
    class MessageHandlerInterface
    {
    public:
        virtual Message* handleReceivedMessage(QString commandName, Message* msg) = 0;
    };
}



#endif /* MESSAGEHANDLERINTERFACE_H_ */
