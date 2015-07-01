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
#include "include/Redeanfrage.hpp"

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
    int enqueue(Redeanfrage *ranf);
    Redeanfrage* dequeue();
    void clear();
};

#endif /* REDEANFRAGEQUEUE_H_ */
