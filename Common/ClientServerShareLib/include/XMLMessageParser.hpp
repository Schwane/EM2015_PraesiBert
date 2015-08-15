/*
 * XMLMessageParser.hpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#ifndef XMLMESSAGEPARSER_HPP_
#define XMLMESSAGEPARSER_HPP_

#include <QObject>
#include <QtCore>
#include <QDateTime>

#include "Message.hpp"

class Client;

//! Parses Messages from XML to Message Object. Capable of separating between data an command/seperation by parent class. Throws signal if message was parsed.
class XMLMessageParser: public QObject
{
    Q_OBJECT
public:
    XMLMessageParser();
    virtual ~XMLMessageParser();
private:
    Message* messageParser(QByteArray& bytes);
public Q_SLOTS:
    void parseMessage(QByteArray bytes);

    void parseCmdMessage(QByteArray bytes);
    void parseDataMessage(QByteArray bytes);
    void parseCmdMessage(QByteArray bytes, uint clientId);
    void parseDataMessage(QByteArray bytes, uint clientId);

Q_SIGNALS:
    void messageParsed(Message *msg);

    void cmdMessageParsed(Message *msg);
    void dataMessageParsed(Message *msg);
    void cmdMessageParsed(Message *msg, uint clientId);
    void dataMessageParsed(Message *msg, uint clientId);


};

#endif /* XMLMESSAGEPARSER_HPP_ */
