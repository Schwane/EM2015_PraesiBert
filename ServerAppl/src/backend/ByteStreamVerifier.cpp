/*
 * ByteStreamVerifier.cpp
 *
 *  Created on: 08.07.2015
 *      Author: sebastian
 */

#include <src/backend/ByteStreamVerifier.h>
#include <src/backend/Logger.h>

namespace ServerAppl
{

    ByteStreamVerifier::ByteStreamVerifier()
    {
        messageAuthenticators = QMap<unsigned int, MessageAuthenticator *> ();
    }

    ByteStreamVerifier::~ByteStreamVerifier()
    {
        // TODO Auto-generated destructor stub
    }

    void ByteStreamVerifier::verifyCmdByteStream(QByteArray messageBytes, uint clientId)
    {
        QByteArray clearMessage = verifyByteStream(clientId, messageBytes);

        if(0 != clearMessage.length())
        {
            WRITE_DEBUG("Verified byte-stream.")
            emit cmdByteStreamVerified(clearMessage, clientId);
        }
        else
        {
            WRITE_DEBUG("Received unprotected byte-stream.")
            emit receivedInvalidByteStream(messageBytes, clientId);
        }
    }

    void ByteStreamVerifier::verifyDataByteStream(QByteArray messageBytes, uint clientId)
    {
        QByteArray clearMessage = verifyByteStream(clientId, messageBytes);

        if(0 != clearMessage.length())
        {
            emit dataByteStreamVerified(clearMessage, clientId);
        }
        else
        {
            emit receivedInvalidByteStream(messageBytes, clientId);
        }
    }

    bool ByteStreamVerifier::addMessageAuthenticator(unsigned int clientId, MessageAuthenticator* authenticator)
    {
        bool successfullyAdded = FALSE;

        if(!messageAuthenticators.contains(clientId))
        {
            messageAuthenticators.insert(clientId, authenticator);
            WRITE_DEBUG("Added message authenticator from ByteStreamValidator.")
        }

        return successfullyAdded;
    }

    bool ByteStreamVerifier::removeMessageAuthenticator(unsigned int clientId)
    {
        bool successfullyRemoved = FALSE;

        if(messageAuthenticators.contains(clientId))
        {
            messageAuthenticators.remove(clientId);
            WRITE_DEBUG("Removed message authenticator from ByteStreamValidator.")
        }

        return successfullyRemoved;
    }

    QByteArray ByteStreamVerifier::verifyByteStream(unsigned int clientId, QByteArray byteStream)
    {
        if(this->messageAuthenticators.contains(clientId))
        {
            QByteArray clearByteStream = byteStream.left(byteStream.length() - 28);
            QByteArray hashByteStream = byteStream.right(28);

            if(this->messageAuthenticators.value(clientId)->hmacSha1(clearByteStream) == hashByteStream)
            {
                return clearByteStream;
            }
            else
            {
                return QByteArray();
            }
        }
        else
        {
            return byteStream;
        }
    }

} /* namespace ServerAppl */
