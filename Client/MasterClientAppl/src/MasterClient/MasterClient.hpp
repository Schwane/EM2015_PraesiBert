/*
 * MasterClient.hpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#ifndef MASTERCLIENT_HPP_
#define MASTERCLIENT_HPP_

#include <QObject>

#include "Client/Client.hpp"
#include "Message/Authentication/MessageAuthenticator.h"

class MasterClient: public Client
{
    enum AuthState
        {
            AUTH_IDLE,
            AUTH_RCV_NONCE,
            AUTH_PROOF,
            AUTH_ACCEPTED,
            AUTH_REJECTED
        };
    Q_OBJECT
public:
    MasterClient();
    virtual ~MasterClient();
public Q_SLOTS:
    Message* loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Q_INVOKABLE void authenticate();
    void connectionLostMaster();
private:
    QByteArray sym_key;
    QByteArray mac_key;
    QString nonce1;
    QString nonce2;
    MessageAuthenticator* msgAuth;
    AuthState auth_state;
};

#endif /* MASTERCLIENT_HPP_ */
