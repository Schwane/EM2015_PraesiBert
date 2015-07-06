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
            REJECTED,
            FINISHED
        };
    Redeanfrage();
    Redeanfrage(QString clientId);
    virtual ~Redeanfrage();

private:
    RedeanfrageState state;
    QString clientId;

public Q_SLOTS:
    void prepare();
    void queue(QString clientId);
    void queue();
    void accept();
    void reject();
    void finish();
    Message* packRedeanfrage();
    QString getClientId();
    void setClientId(QString clientId);


Q_SIGNALS:
/*
    void queued();
    void accepted();
    void rejected();
*/
    void stateChanged(QString state);


};

#endif /* REDEANFRAGE_HPP_ */
