/*
 * Master.h
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <QObject>

#include <src/backend/UnspecifiedClient.h>
#include <src/backend/MessageHandlerInterface.h>

typedef struct{
    QString part_1;
    QString part_2;
}NONCE;

namespace ServerAppl
{
    class Server;

    class Master : public UnspecifiedClient
    {
        Q_OBJECT

    public:
        static bool createMaster(UnspecifiedClient * client, Master * master, QString nonce1);

        Master();
        Master(UnspecifiedClient * priorClientObject, QString nonce1);
        virtual ~Master();

        Message* handleReceivedMessage(QString commandName, Message* msg);
        Message* handleProofResponse(QString commandName, Message* msg);
        Message* handleLoginResponse(QString commandName, Message* msg);

    signals:
        void receivedSlides();

    public slots:
        void onReceivedData(unsigned int receiverIdentifier);
        void onTransmitSlidesResponse(bool accepted);

    private:
        UnspecifiedClient * priorClientObject;
        NONCE nonce;
        bool acceptSlides;
    };
} /* namespace ServerAppl */

#endif /* MASTER_H_ */
