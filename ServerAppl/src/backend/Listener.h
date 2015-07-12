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
        static Listener*  createListener(UnspecifiedClient* client);
        Listener(UnspecifiedClient * priorClientObject);
        Listener();
        virtual ~Listener();
        void setHasPresentation(bool hasPresentation);
        bool getHasPresentation();

        /* Message handlers */
        Message* handleReceivedMessage(QString commandName, Message* msg);
//        Message* handleLoginAcknowledge(QString commandName, Message* msg);

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

        bool hasPresentation;
    };
} /* namespace ServerAppl */

#endif /* LISTENER_H_ */
