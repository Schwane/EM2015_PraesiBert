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
        void createMessageByteArray(Message* msg, QByteArray* outputMessageBytes);
        //QXmlStreamReader xmlr;
        //Client* cl;
    public Q_SLOTS:
        void writeMessage(Message* msg);    //TODO: needs to be replaced because Server/Client-sockets have different signals for cmd-/data-messages.

        void writeCmdMessage(Message* msg);
        void writeDataMessage(Message* msg);
        void writeCmdMessage(Message* msg, uint clientId);
        void writeDataMessage(Message* msg, uint clientId);

    Q_SIGNALS:
        void messageWritten(QByteArray msg);    //TODO: needs to be replaced because Server/Client-sockets have different slots for cmd-/data-messages.

        void cmdMessageWritten(QByteArray msg);
        void dataMessageWritten(QByteArray msg);
        void cmdMessageWritten(QByteArray msg, uint clientId);
        void dataMessageWritten(QByteArray msg, uint clientId);
};

#endif /* XMLMESSAGEWRITER_HPP_ */
