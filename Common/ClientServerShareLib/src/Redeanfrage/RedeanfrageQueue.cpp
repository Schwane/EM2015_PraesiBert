/*
 * RedeanfrageQueue.cpp
 *
 *  Created on: 01.07.2015
 *      Author: JCB
 */

#include "include/RedeanfrageQueue.hpp"

RedeanfrageQueue::RedeanfrageQueue()
{

}

RedeanfrageQueue::~RedeanfrageQueue()
{

}


int
RedeanfrageQueue::enqueue(Redeanfrage *ranf)
{
    int ret = 1;
    int sc = 0;
    accessLock.lock();
    //check if already contained (by client id)
    bool alreadyContained = false;

    for (int i = 0; i < queue.size(); i++)
    {
        if (queue.at(i)->getClientId() == ranf->getClientId())
            alreadyContained = true;
    }

    //only enqueue if not already contained (only one request per user!!!)
    if (alreadyContained)
        ret = 0;
    else
    {
        queue.enqueue(ranf);
        sc = 1;
    }
    accessLock.unlock();
    if (sc)
        emit sizeChanged(queue.size()); //only emit if size changed
    return ret;
}
Redeanfrage*
RedeanfrageQueue::dequeue()
{
    Redeanfrage *ret = NULL;
    int sc = 0;
    accessLock.lock();
    //only dequeue if queue is not empty
    if (!queue.isEmpty())
    {
        ret = queue.dequeue();
        sc = 1;
    }
    accessLock.unlock();
    if (sc)
        emit sizeChanged(queue.size());
    return ret;
}

void
RedeanfrageQueue::clear()
{
    accessLock.lock();
    //clean up the queue first
    for (int i = 0; i < queue.size(); i++)
        delete queue.at(i);
    //clear it afterwards
    queue.clear();
    accessLock.unlock();
    emit sizeChanged(queue.size());
}

int
RedeanfrageQueue::getSize()
{
    int ret = 0;
    accessLock.lock();
    //return size of the queue
    ret = queue.size();
    accessLock.unlock();
    return ret;
}

QString
RedeanfrageQueue::getClientIdAt(int i)
{
    QString ret = 0;
    accessLock.lock();
    //return client id at position i in the queue
    ret = queue.at(i)->getClientId();
    accessLock.unlock();
    return ret;
}
