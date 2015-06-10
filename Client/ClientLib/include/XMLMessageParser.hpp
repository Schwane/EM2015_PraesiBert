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
    //QXmlStreamReader xmlr;
    //Client* cl;
public Q_SLOTS:
    void parseMessage(QByteArray bytes);
Q_SIGNALS:
    void messageParsed(Message *msg);
};

#endif /* XMLMESSAGEPARSER_HPP_ */
