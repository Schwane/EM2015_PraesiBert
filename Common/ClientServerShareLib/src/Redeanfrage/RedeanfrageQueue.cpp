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
    if (queue.indexOf(ranf, 0) > -1)
        ret = 0;
    else
    {
        queue.enqueue(ranf);
        sc = 1;
    }
    accessLock.unlock();
    if (sc)
        emit sizeChanged(queue.size());
    return ret;
}
Redeanfrage*
RedeanfrageQueue::dequeue()
{
    Redeanfrage *ret = NULL;
    int sc = 0;
    accessLock.lock();
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
    queue.clear();
    accessLock.unlock();
    emit sizeChanged(queue.size());
}
