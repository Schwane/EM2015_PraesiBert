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
    accessLock.lock();
    if (queue.indexOf(ranf, 0) > -1)
        ret = 0;
    else
        queue.enqueue(ranf);
    accessLock.unlock();
    return ret;
}
Redeanfrage*
RedeanfrageQueue::dequeue()
{
    Redeanfrage *ret = NULL;

    accessLock.lock();
    if (!queue.isEmpty())
        ret = queue.dequeue();
    accessLock.unlock();
    return ret;
}
