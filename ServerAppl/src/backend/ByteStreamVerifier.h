/*
 * ByteStreamVerifier.h
 *
 *  Created on: 08.07.2015
 *      Author: sebastian
 */

#ifndef BYTESTREAMVERIFIER_H_
#define BYTESTREAMVERIFIER_H_

//#include <QByteArray.h>
#include <QMap>
#include <QObject>

#include <MessageAuthenticator.h>

namespace ServerAppl
{

    class ByteStreamVerifier : public QObject
    {
        Q_OBJECT

    public:
        ByteStreamVerifier();
        virtual ~ByteStreamVerifier();
        bool addMessageAuthenticator(unsigned int clientId, MessageAuthenticator * authenticator);
        bool removeMessageAuthenticator(unsigned int clientId);

    signals:
        void cmdByteStreamVerified(QByteArray messageBytes, uint clientId);
        void dataByteStreamVerified(QByteArray messageBytes, uint clientId);
        void receivedInvalidByteStream(QByteArray bytes, uint clientId);

    public slots:
        void verifyCmdByteStream(QByteArray messageBytes, uint clientId);
        void verifyDataByteStream(QByteArray messageBytes, uint clientId);

    private:
        QByteArray verifyByteStream(unsigned int clientId, QByteArray byteStream);

        QMap<unsigned int, MessageAuthenticator *> messageAuthenticators;
    };

} /* namespace ServerAppl */

#endif /* BYTESTREAMVERIFIER_H_ */
