/*
 * Redeanfrage.hpp
 *
 *  Created on: 01.07.2015
 *      Author: JCB
 */

#ifndef REDEANFRAGE_HPP_
#define REDEANFRAGE_HPP_

#include <QObject>
#include "Message.hpp"
#include "commands.hpp"

class Redeanfrage: public QObject
{
    Q_OBJECT
public:
    enum RedeanfrageState
        {
            PREPARATION,
            QUEUED,
            ACCEPTED,
            REJECTED
        };
    Redeanfrage();
    Redeanfrage(QString clientId);
    virtual ~Redeanfrage();

private:
    RedeanfrageState state;
    QString clientId;

public Q_SLOTS:
    void queue(QString clientId);
    void queue();
    void accept();
    void reject();
    Message* packRedeanfrage();
    QString getClientId();


Q_SIGNALS:
    void queued();
    void accepted();
    void rejected();



};

#endif /* REDEANFRAGE_HPP_ */
