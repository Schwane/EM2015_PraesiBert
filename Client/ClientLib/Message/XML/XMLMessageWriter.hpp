/*
 * XMLMessageWriter.hpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#ifndef XMLMESSAGEWRITER_HPP_
#define XMLMESSAGEWRITER_HPP_

#include <QObject>
#include <QtCore>
#include "Message.hpp"

class Message;

class XMLMessageWriter: public QObject
{
    Q_OBJECT
public:
    XMLMessageWriter();
    //XMLMessageWriter(Client *cl);
    virtual ~XMLMessageWriter();
    private:
        //QXmlStreamReader xmlr;
        //Client* cl;
    public Q_SLOTS:
        void writeMessage(Message* msg, bool cleanup);
        void writeMessage(Message* msg);
    Q_SIGNALS:
        void messageWritten(QByteArray msg);
};

#endif /* XMLMESSAGEWRITER_HPP_ */
