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

    /**
     * @class ByteStreamVerifier ByteStreamVerifier.h "src/backend/ByteStreamVerifier.h"
     *
     * @brief An object of this class can verify a QByteStream that has a hash at its and.
     *
     * This class implements the verification of received byte-streams from a specific client. Therefore it has a list with clientId's
     * and corresponding MessageAuthenticator-objects. If a byte-stream from a client with an ID from that table was received
     * the ByteStreamVerifier will try to check if the byte-stream is valid. If the byte-stream is not valid it will be deleted.
     * Otherwise it will be forwarded.
     * If no MessageAuthenticator for a clientId exist the byte-stream will always be forwarded.
     *
     */
    class ByteStreamVerifier : public QObject
    {
        Q_OBJECT

    public:
        ByteStreamVerifier();
        virtual ~ByteStreamVerifier();

        /**
         * @brief   Adds a MessageAuthenticator-object for a specific client.
         */
        bool addMessageAuthenticator(unsigned int clientId, MessageAuthenticator * authenticator);

        /**
         * @brief   Removes a MessageAuthenticator-object for a specific client.
         *
         * @return  True if a MessageAuthenticator exists for the client and was successfully removed.
         *
         * The object will NOT be deleted!
         */
        bool removeMessageAuthenticator(unsigned int clientId);

    signals:
        /**
         * @brief   Will be emitted if a byte-stream from a command-port was successfully verified.
         */
        void cmdByteStreamVerified(QByteArray messageBytes, uint clientId);

        /**
         * @brief   Will be emitted if a byte-stream from a data-port was successfully verified.
         */
        void dataByteStreamVerified(QByteArray messageBytes, uint clientId);

        /**
         * @brief   Will be emitted if a byte-stream could not be verified.
         */
        void receivedInvalidByteStream(QByteArray bytes, uint clientId);

    public slots:
        /**
         * @brief   Takes a received command-byte-stream and tries to verify it with a MessageAuthenticator.
         */
        void verifyCmdByteStream(QByteArray messageBytes, uint clientId);

        /**
         * @brief   Takes a received data-byte-stream and tries to verify it with a MessageAuthenticator.
         */
        void verifyDataByteStream(QByteArray messageBytes, uint clientId);

    private:
        QByteArray verifyByteStream(unsigned int clientId, QByteArray byteStream);

        QMap<unsigned int, MessageAuthenticator *> messageAuthenticators;
    };

} /* namespace ServerAppl */

#endif /* BYTESTREAMVERIFIER_H_ */
