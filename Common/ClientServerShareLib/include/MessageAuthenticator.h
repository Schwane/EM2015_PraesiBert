/*
 * MessageAuthenticator.h
 *
 *  Created on: 01.06.2015
 *      Author: Jan Zimmer
 */

#ifndef MESSAGEAUTHENTICATOR_H_
#define MESSAGEAUTHENTICATOR_H_

#include <QObject>
#include <QCryptographicHash>

//! Provides a transparent interface to authenticate messages via HMAC given a key.
/*!
 * Provides a transparent interface to authenticate messages via HMAC given a key.
 * Key has to be set before authentication.
 * In addition the class provides HMAC algorithm.
 * It accepts raw byte data (DATA) via slot and returns raw data concatenated with 28 byte base64 encoded mac (DATA||MAC) via signal.
 * Example: For the Masterclient the Authenticator is placed between XML Writer and network layer (via reconnecting signals and slots) so every message automatically gets mac'ed.
 */
class MessageAuthenticator: public QObject
{
    Q_OBJECT
public:
    MessageAuthenticator();
    virtual ~MessageAuthenticator();
    QByteArray hmacSha1(QByteArray baseString);
    QByteArray hmacSha1(QByteArray key, QByteArray baseString);
public Q_SLOTS:
    void authenticateMessage(QByteArray msg);
    void setKey(QByteArray key);
Q_SIGNALS:
    void messageAuthenticated(QByteArray msg);
private:
    QByteArray key;

};

#endif /* MESSAGEAUTHENTICATOR_H_ */
