/*
 * Master.h
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <QByteArray>
#include <QObject>
#include <QTimer>

#include <Praesentation.hpp>
#include <MessageAuthenticator.h>

#include <src/backend/UnspecifiedClient.h>
#include <src/backend/MessageHandlerInterface.h>

typedef struct{
    QString part_1;
    QString part_2;
}NONCE;

enum MasterAuthenticationState
{
    PHASE_1,    /* received nonce 1 */
    PHASE_2,    /* transmitted nonce 2 */
    PHASE_3,    /* received message to proof working authentication */
    ACCEPTED,   /* received acknowledge */
    REJECTED
};

enum MasterAuthenticationEvent
{
    ReceivedPhase1Message,
    TransmittedPhase2Message,
    ReceivedPhase3Message,
    ReceivedAcknowledgeMessage,
    ReceivedCorruptAuthenticationMessage
};

namespace ServerAppl
{
    class Server;

    /**
     * @class Master Master.h "src/backend/Master.h"
     *
     * @brief Objects of this class represent a master-client.
     *
     * Objects of this class will represent a connected master-client. Some of the commands from the master will be handled by this
     * class (e.g. authentication-commands, set-slide-commands or receiving data from the master-client).
     */
    class Master : public UnspecifiedClient
    {
        Q_OBJECT

    public:
        /**
         * @brief   Static function to generate a Master-object basing on an UnspecifiedClient-object and a received nonce (part 1).
         *
         * @return  A pointer to a new Master-object will be returned (may be NULL!)
         */
        static Master* createMaster(UnspecifiedClient * client, QString nonce1);

        /**
         * @brief   A function to generate a nonce (e.g. nonce parte 2) basing on a seed.
         *
         * @brief   Returns a string with a nonce.
         */
        static QString generateNonce(uint seed);

        Master();
        Master(UnspecifiedClient * priorClientObject, QString nonce1);
        virtual ~Master();

        /**
         * @brief   Returns the complete nonce of the authentication-process.
         *
         * @return  Returns the nonce which was transmitted by the master-client and the part that was generated on the server.
         */
        NONCE getNonce();

        /**
         * @brief   Returns a MessageAuthenticator-object which can be used to verify received messages from the master-client.
         */
        MessageAuthenticator * getMessageAuthenticator();

        /**
         * @brief   Returns the type of this client (for this class always ClientType_Master).
         */
        ClientType getClientType();

        /**
         * @brief   This function contains a state-machine for the authentication-process.
         *
         * @param[in] event An event for the state-machine.
         *
         * @return  The new state of the state-machine.
         *
         * This function can be used to ensure a correct authentication-process. Just give an event to the state-machine and
         * check the returned state. If the returned state is the same as the expected new state everything is fine.
         */
        MasterAuthenticationState authenticationStm(MasterAuthenticationEvent event);

        /* Message-handlers */
        Message* handleUnknownMessage(QString commandName, Message* msg);

        /**
         * @brief   Handles phase-3-authentication-command.
         * The received message-object (msg) will be deleted.
         */
        Message* handleAuthenticationPhase3(QString commandName, Message* msg);

        /**
         * @brief   Handles an authentication-acknowledge-command.
         *
         * This command will be transmitted by the master after the server responded on the phase-3-command.
         * The received message-object (msg) will be deleted.
         */
        Message* handleAuthenticationAcknowledge(QString commandName, Message* msg);

        /**
         * @brief   This message-handler will save a received presentation.
         *
         * Usually this message-handler shall be registered for a data-port.
         * If a presentation was received the server will be informed (signal receivedPresentation). The new
         * presentation will be given to the Server-object. The Server-object is responsible for the distribution of the
         * new presentation to the listener-clients.
         * The received message-object (msg) will be deleted.
         */
        Message* handleDataPresentation(QString commandName, Message* msg);

        /**
         * @brief   This handles a set-slide-command of the master-client.
         *
         * The server-object will be informed by the signal receivedSetSlide. The server-object is responsible for the
         * forwarding of this command to the clients.
         * The received message-object (msg) will be deleted.
         */
        Message* handleSetSlide(QString commandName, Message* msg);

        /**
         * @brief   Handles a stop-presentation-command.
         *
         * The Server-object will be informed of this command. The master-object will take no further actions.
         * The received message-object (msg) will be deleted.
         */
        Message* handleStopPresentation(QString commandName, Message* msg);

        /**
         * @brief   Handles a received audio-transmission of the master-client.
         *
         * The audio-file will be given to the Server-object with the signal writeAudioRecording.
         * The received message-object (msg) will be deleted.
         */
        Message* handleReceivedAudio(QString commandName, Message* msg);

    signals:
        void stopPresentation();
        void forwardMessageToClient(Message * msg, unsigned int clientId);
        void receivedSlides();
        void receivedSetSlide(int slideNumber);
        void authenticationFailed();
        void authenticationSuccessfull();
        void receivedPresentation(Praesentation * presentation, QMap<QString, QVariant> presentationParameterList, QMap<QString, QString> presentationParameterTypeList);
        void writeAudioRecording(QString fileName, const QByteArray & recording);

    public slots:
        void onReceivedData(unsigned int receiverIdentifier);
        void onTransmitSlidesResponse(bool accepted);
        void authenticationTimeout();

    private:
        UnspecifiedClient * priorClientObject;
        NONCE nonce;
        QByteArray symmetricKey;
        QByteArray macKey;
        MessageAuthenticator * messageAuthenticator;
        MasterAuthenticationState currentAuthState;
        bool acceptSlides;
        QTimer * authenticationTimer;

    };
} /* namespace ServerAppl */

#endif /* MASTER_H_ */
