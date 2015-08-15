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

//! Class containing one talk request and its state.
/*!
 * Implementation of talk requests.
 * Contains the state of the request as well as operations to change state or send the request.
 * For detailed information see documentation's state diagram.
 */
class Redeanfrage: public QObject
{
    Q_OBJECT
public:
    enum RedeanfrageState
        {
            PREPARATION, /*!< Preperation state */
            QUEUED, /*!< queued and waiting for answer */
            ACCEPTED, /*!< answer was an accept */
            REJECTED, /*!< answer was a reject */
            FINISHED /*!< successfully finish talk request after accept */
        };
    Redeanfrage();
    Redeanfrage(QString clientId);
    virtual ~Redeanfrage();

private:
    RedeanfrageState state;
    QString clientId;

public Q_SLOTS:
    //! Prepare talk request.
    void prepare();
    //! Queue talk request and set client id at once.
    void queue(QString clientId);
    //! Queue talk request.
    void queue();
    //! Accept talk request.
    void accept();
    //! Reject talk request.
    void reject();
    //! Finish talk request.
    void finish();
    //! Pack talk request for sending.
    Message* packRedeanfrage();
    //! Returns client id.
    QString getClientId();
    //! Set client id. necessary if client id is not known at creation time.
    void setClientId(QString clientId);


Q_SIGNALS:
/*
    void queued();
    void accepted();
    void rejected();
*/
    //! Signal if state has changed.
    void stateChanged(QString state);


};

#endif /* REDEANFRAGE_HPP_ */
