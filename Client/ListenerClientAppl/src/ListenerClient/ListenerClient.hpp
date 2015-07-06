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

class ListenerClient: public Client
{
    Q_OBJECT
public:
    ListenerClient();
    virtual ~ListenerClient();
public Q_SLOTS:
    /*Remote execution functions*/
    Message* redeanfrageResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Message* redeanfrageFinish(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);

    Q_INVOKABLE void doRanf();
    Q_INVOKABLE void acceptRanf();
    Q_INVOKABLE void rejectRanf();
    void onLogin();
    void onRanfstateChanged(QString state);
Q_SIGNALS:
    void ranfStateChanged(QString state);
    void ranfAnswer();
private:
    Redeanfrage *ranf;
};

#endif /* LISTENERCLIENT_HPP_ */
