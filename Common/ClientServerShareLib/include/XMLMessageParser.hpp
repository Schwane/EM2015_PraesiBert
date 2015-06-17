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

class XMLMessageParser: public QObject
{
    Q_OBJECT
public:
    XMLMessageParser();
    //XMLMessageParser(Client *cl);
    virtual ~XMLMessageParser();
private:
    Message* messageParser(QByteArray& bytes);
    //QXmlStreamReader xmlr;
    //Client* cl;
public Q_SLOTS:
    void parseMessage(QByteArray bytes); //TODO: should be removed because it don't seperates between data and command messages

    void parseCmdMessage(QByteArray bytes);
    void parseDataMessage(QByteArray bytes);
    void parseCmdMessage(QByteArray bytes, uint clientId);
    void parseDataMessage(QByteArray bytes, uint clientId);

Q_SIGNALS:
    void messageParsed(Message *msg); //TODO: should be removed because it don't seperates between data and command messages

    void cmdMessageParsed(Message *msg);
    void dataMessageParsed(Message *msg);
    void cmdMessageParsed(Message *msg, uint clientId);
    void dataMessageParsed(Message *msg, uint clientId);


};

#endif /* XMLMESSAGEPARSER_HPP_ */
