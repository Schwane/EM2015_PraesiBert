/*
 * MasterClient.hpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#ifndef MASTERCLIENT_HPP_
#define MASTERCLIENT_HPP_

#include <QObject>

#include "Client.hpp"
#include "MessageAuthenticator.h"
#include "commands.hpp"
#include "Redeanfrage.hpp"
#include "RedeanfrageQueue.hpp"

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
    /*Remote execution functions*/
    Message* loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Message* redeanfrage(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Message* redeanfrageFinal(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);

    Q_INVOKABLE void authenticate();
    void connectionLostMaster();

    Q_INVOKABLE void clearRanf();
    Q_INVOKABLE void muteRanf();
    Q_INVOKABLE void acceptRanf();
    Q_INVOKABLE void dummyRanf();
    Q_INVOKABLE void finishRanf();

    Q_INVOKABLE void setKey(QString key);
Q_SIGNALS:
    void ranfMuteChanged(bool mute);
    void ranfSizeChanged(int size);
    void ranfFinalAnswer(QString answ);
private:
    QByteArray sym_key;
    QByteArray mac_key;
    QString nonce1;
    QString nonce2;
    MessageAuthenticator* msgAuth;
    AuthState auth_state;

    RedeanfrageQueue *ranf_queue;
    Redeanfrage *current_ranf;
    bool ranf_mute;
    int ranf_size;
};

#endif /* MASTERCLIENT_HPP_ */
