/*
 * Client.h
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <QObject>
#include <QString>
#include <QTime>

namespace ServerAppl
{

    class Client : public QObject
    {
        Q_OBJECT

    public:
        Client();
        virtual ~Client();
        unsigned int getIdentifier();

    private:
        static unsigned int clientCounter;

        QString * name;
        QTime * lastTimestamp;
        unsigned int identifier;
    };

} /* namespace ServerAppl */

#endif /* CLIENT_H_ */
