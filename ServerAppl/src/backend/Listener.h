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
        ClientType getClientType();

        /* Message handlers */
        Message* handleUnknownMessage(QString commandName, Message* msg);
        Message* handleAcknowledge(QString commandName, Message* msg);
        Message* handleReceivedAudio(QString commandName, Message* msg);

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

        signals:
            void forwaredMessageToMaster(Message * msg, unsigned int clientId);
            void requestDeliverPresentation(unsigned int clientId);
            void writeAudioRecording(QString fileName, const QByteArray & recording);

    private:
        UnspecifiedClient * priorClientObject;
        bool connectionStm(ListenerConnectionStmEvent);
        bool hasPresentation;
        bool loginAcknowledged;
    };
} /* namespace ServerAppl */

#endif /* LISTENER_H_ */
