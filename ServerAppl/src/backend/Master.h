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

    class Master : public UnspecifiedClient
    {
        Q_OBJECT

    public:
        static Master* createMaster(UnspecifiedClient * client, QString nonce1);
        static QString generateNonce(uint seed);

        Master();
        Master(UnspecifiedClient * priorClientObject, QString nonce1);
        virtual ~Master();
        NONCE getNonce();
        MessageAuthenticator * getMessageAuthenticator();
        MasterAuthenticationState authenticationStm(MasterAuthenticationEvent event);

        Message* handleReceivedMessage(QString commandName, Message* msg);
        Message* handleAuthenticationPhase3(QString commandName, Message* msg);
        Message* handleAuthenticationAcknowledge(QString commandName, Message* msg);
        Message* handleDataPresentation(QString commandName, Message* msg);

    signals:
        void receivedSlides();
        void authenticationFailed();
        void authenticationSuccessfull();
        void receivedPresentation(Praesentation * presentation, QMap<QString, QVariant> presentationParameterList, QMap<QString, QString> presentationParameterTypeList);

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
