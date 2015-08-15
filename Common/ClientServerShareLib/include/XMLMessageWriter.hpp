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
//! Serializes Message object to XML. Capable of separating between data an command/seperation by parent class. Throws signal if message was written.
class XMLMessageWriter: public QObject
{
    Q_OBJECT
public:
    XMLMessageWriter();
    virtual ~XMLMessageWriter();
    private:
        void createMessageByteArray(Message* msg, QByteArray* outputMessageBytes);
    public Q_SLOTS:
        void writeMessage(Message* msg);

        void writeCmdMessage(Message* msg);
        void writeDataMessage(Message* msg);
        void writeCmdMessage(Message* msg, uint clientId);
        void writeDataMessage(Message* msg, uint clientId);
        void writeCmdMessage(Message* msg, QList<uint> clientIDs);
        void writeDataMessage(Message* msg, QList<uint> clientIDs);

    Q_SIGNALS:
        void messageWritten(QByteArray msg);

        void cmdMessageWritten(QByteArray msg);
        void dataMessageWritten(QByteArray msg);
        void cmdMessageWritten(QByteArray msg, uint clientId);
        void dataMessageWritten(QByteArray msg, uint clientId);
        void cmdMessageWritten(QByteArray msg, QList<uint> clientIDs);
        void dataMessageWritten(QByteArray msg, QList<uint> clientIDs);
};

#endif /* XMLMESSAGEWRITER_HPP_ */
