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

#define IS_COMMAND(cmdObj, cmdStr) ((cmdStr) == (cmdObj))
#define HANDLER_OBJ(obj) ((MessageHandlerInterface *)(obj))
#define HANDLER_FUNC(func) ( static_cast<handleReceivedMessageFunction>(&func) )

namespace ServerAppl
{
    class MessageHandlerInterface
    {
    public:
        virtual Message* handleUnknownMessage(QString commandName, Message* msg) = 0;
    };
}



#endif /* MESSAGEHANDLERINTERFACE_H_ */
