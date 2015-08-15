/*
 * ListenerClient.hpp
 *
 *  Created on: 06.07.2015
 *      Author: JCB
 */

#ifndef LISTENERCLIENT_HPP_
#define LISTENERCLIENT_HPP_

#include <QObject>

#include "Client.hpp"
#include "commands.hpp"
#include "Redeanfrage.hpp"
//! Implements extra functionality of ListenerClient.
/*!
 * Implements extra functionality of ListenerClient, mainly the possibility to do talk-requests.
 */
class ListenerClient: public Client
{
    Q_OBJECT
public:
    ListenerClient();
    virtual ~ListenerClient();
public Q_SLOTS:
    /*Remote execution functions*/
    //! Handle Response to talk-request.
    Message* redeanfrageResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //!Handle finishing of talk-request.
    Message* redeanfrageFinish(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);

    //! Initiates talk-request.
    Q_INVOKABLE void doRanf();
    //! talk-request is relevant
    Q_INVOKABLE void acceptRanf();
    //! talk request is not relevant anymore
    Q_INVOKABLE void rejectRanf();

Q_SIGNALS:
    //! Rethrows signal of talk-request.
    void ranfStateChanged(QString state);
    void ranfAnswer();
private:
    //! Contains the actual talk-request.
    Redeanfrage *ranf;
};

#endif /* LISTENERCLIENT_HPP_ */
