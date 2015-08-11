/*
 * RedeanfrageQueue.h
 *
 *  Created on: 01.07.2015
 *      Author: JCB
 */

#ifndef REDEANFRAGEQUEUE_H_
#define REDEANFRAGEQUEUE_H_

#include <QObject>
#include <QtCore>
#include "Redeanfrage.hpp"

//! Thread safe implementation of a queue containing talk requests.
/*! Thread safe implementation of a queue containing talk requests.
 * Distinct enqueueing per default.
 */
class RedeanfrageQueue: public QObject
{
Q_OBJECT
public:
    RedeanfrageQueue();
    virtual ~RedeanfrageQueue();
private:
    QMutex accessLock;
    QQueue<Redeanfrage*> queue;
public Q_SLOTS:
    //! Only enqueue element if its not already contained.
    int enqueue(Redeanfrage *ranf);
    Redeanfrage* dequeue();
    void clear();
    int getSize();
    QString getClientIdAt(int i);
Q_SIGNALS:
    void sizeChanged(int size);
};

#endif /* REDEANFRAGEQUEUE_H_ */
