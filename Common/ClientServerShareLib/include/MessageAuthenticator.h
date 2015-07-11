/*
 * MessageAuthenticator.h
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#ifndef MESSAGEAUTHENTICATOR_H_
#define MESSAGEAUTHENTICATOR_H_

#include <QObject>
#include <QCryptographicHash>

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
