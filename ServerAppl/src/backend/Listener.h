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
    /**
     * @class Listener Listener.h "src/backend/Listener.h"
     *
     * @brief Objects of this class represent a listener-client.
     *
     * Objects of this class will represent a connected listener-client. It will be used mainly to hold some information about the
     * listener (e.g. if already a presentation was delivered) and to receive audio-data from the listener.
     */
    class Listener : public UnspecifiedClient
    {
        Q_OBJECT

    public:
        /**
         * @brief   Static function to generate a Listener-object basing on an UnspecifiedClient-object.
         *
         * @return  A pointer to a new Listener-object will be returned (may be NULL!)
         */
        static Listener*  createListener(UnspecifiedClient* client);
        Listener(UnspecifiedClient * priorClientObject);
        Listener();
        virtual ~Listener();

        /**
         * @brief   Sets an indicator whether a presentation had been transmitted to the listener.
         *
         * @param[in] hasPresentation   True if a presentation was transmitted.
         */
        void setHasPresentation(bool hasPresentation);

        /**
         * @brief   Indicates whether already a presentation has been transmitted to the listener.
         *
         * @return  True if a presentation was transmitted.
         */
        bool getHasPresentation();

        /**
         * @brief   Returns the type of this client (for this class always ClientType_Lister).
         */
        ClientType getClientType();

        /* Message handlers */
        Message* handleUnknownMessage(QString commandName, Message* msg);
        Message* handleAcknowledge(QString commandName, Message* msg);

        /**
         * @brief   Handles incomming audio-transmissions from the listener-client.
         *
         * The listener-shall transmit its audio-recordings over the data-port from a talk-request (Redeanfrage) to the
         * server when the talk-request is finished.
         * This message-handler emits the signal writeAudioRecording with the received audio-data as a QByteStream.
         * The Server-object shall save the data afterwards to the file-system
         */
        Message* handleReceivedAudio(QString commandName, Message* msg);

        signals:
            void forwaredMessageToMaster(Message * msg, unsigned int clientId);
            void requestDeliverPresentation(unsigned int clientId);
            void writeAudioRecording(QString fileName, const QByteArray & recording);

    private:
        UnspecifiedClient * priorClientObject;
        bool hasPresentation;
        bool loginAcknowledged;
    };
} /* namespace ServerAppl */

#endif /* LISTENER_H_ */
