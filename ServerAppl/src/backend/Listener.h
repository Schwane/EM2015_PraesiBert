/*
 * Listener.h
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include <QObject>

#include <Message.hpp>

#include <src/backend/UnspecifiedClient.h>
#include <src/backend/MessageHandlerInterface.h>

namespace ServerAppl
{
    class Listener : public UnspecifiedClient
    {
        Q_OBJECT

    public:
        static bool createListener( UnspecifiedClient * client, Listener * listener);
        Listener(UnspecifiedClient * priorClientObject);
        Listener();
        virtual ~Listener();

        Message* handleReceivedMessage(QString commandName, Message* msg);

        /* data types */
        enum ListenerConnectionStmState
        {
            NotInitialized,
            NetworkConnectionEstablished,
            WaitingForSlides,
            ReceivingSlides,
            Ready
        };

        enum ListenerConnectionStmEvent
        {
            Init,
            Login,
            StartTransmittingSlides,
            ReceivedSlides,
            StartPresentation,
            StopPresentation,
            ShowOtherSlide
        };


    private:
        UnspecifiedClient * priorClientObject;
        bool connectionStm(ListenerConnectionStmEvent);
    };
} /* namespace ServerAppl */

#endif /* LISTENER_H_ */
