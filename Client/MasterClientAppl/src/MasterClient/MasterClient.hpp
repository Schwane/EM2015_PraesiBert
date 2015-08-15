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
#include "CameraController.hpp"
//! Implements extra functionality of MasterClient.
/*!
 * Implements extra functionality of MasterClient, mainly the possibility to queue and manage talk-requests, authentication and presentation controlling plus geasture control.
 */
class MasterClient: public Client
{
    //! Authentication state
    enum AuthState
        {
            AUTH_IDLE, /*!< not authenticated */
            AUTH_RCV_NONCE, /*!< waiting for the second nonce */
            AUTH_PROOF,/*!< proofed, waiting for answer */
            AUTH_ACCEPTED,/*!< authenticated */
            AUTH_REJECTED/*!<rejected */
        };
    Q_OBJECT
public:
    MasterClient();
    virtual ~MasterClient();
public Q_SLOTS:
    /*Remote execution functions*/
    //! react on authentication messages
    Message* loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //! handle talk-request
    Message* redeanfrage(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //! auto-reject talk-request
    Message* redeanfrageAutoReject(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //! handle if talk-request was relevant or not
    Message* redeanfrageFinal(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);

    //! initiate authentication
    Q_INVOKABLE void authenticate();
    void connectionLostMaster();

    //! clear talk-requests
    Q_INVOKABLE void clearRanf();
    //! mute talk-requests
    Q_INVOKABLE void muteRanf();
    //! accept talk-request
    Q_INVOKABLE void acceptRanf();
    //! finish talk-request
    Q_INVOKABLE void finishRanf();

    //! set the session key
    Q_INVOKABLE void setKey(QString key);

    //! read selected presentation from path
    Q_INVOKABLE void selectPraesentation(QString path);
    //! deliver presentation to server
    Q_INVOKABLE void deliverPraesentation();
    //! request a stop of the presentation
    Q_INVOKABLE void requestStopPraesentation();

    //! activate geasture controll
    Q_INVOKABLE void activateGesture(bool active);
Q_SIGNALS:
    //! thrown if talk-reqeust state of current request changed
    void ranfMuteChanged(bool mute);
    //! thrown if queue size changed
    void ranfSizeChanged(int size);
    //! thrown if relevance information arrived
    void ranfFinalAnswer(QString answ);
    //! thrown to idicate presentation state
    void praesentationRunning(bool active);
private:
    //! password
    QByteArray sym_key;
    //! mac key
    QByteArray mac_key;
    //! nonce 1
    QString nonce1;
    //! nonce 2
    QString nonce2;
    //! Authenticator for commands
    MessageAuthenticator* msgAuthCmd;
    //! Authentication for data
    MessageAuthenticator* msgAuthData;
    //! state of authentication
    AuthState auth_state;

    //! queue for talk-requests
    RedeanfrageQueue *ranf_queue;
    //! current talk-request
    Redeanfrage *current_ranf;
    //! indicates whether to auto-reject talk-requests or not
    bool ranf_mute;
    //! indicates number of outstanding requests
    int ranf_size;

    //! geasturecontrol
    CameraController *cc;
};

#endif /* MASTERCLIENT_HPP_ */
