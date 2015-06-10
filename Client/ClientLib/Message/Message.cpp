/*
 * Message.cpp
 *
 *  Created on: 22.05.2015
 *      Author: JCB
 */

#include "Message.hpp"

Message::Message(QString command, QString sender, QString receiver)
{
    this -> command = command;
    this -> sender = sender;
    this -> receiver = receiver;
    this -> timestamp = QDateTime();

}

Message::Message()
{

}

Message::~Message()
{
    // TODO Auto-generated destructor stub
}


int
Message::addParameter(QString name, QString value)
{
    if (parameters.contains(name))
    {
        return false;
    }
    parameters.insert(name, value);
    parameter_types.insert(name, "string");
    return 1;

}

int
Message::addParameter(QString name, QDateTime value)
{
    if (parameters.contains(name))
    {
        return false;
    }
    parameters.insert(name, value);
    parameter_types.insert(name, "date");
    return 1;

}

int
Message::addParameter(QString name, int value)
{
    if (parameters.contains(name))
    {
        return false;
    }
    parameters.insert(name, value);
    parameter_types.insert(name, "integer");
    return 1;

}

int
Message::addParameter(QString name, double value)
{
    if (parameters.contains(name))
    {
        return false;
    }
    parameters.insert(name, value);
    parameter_types.insert(name, "decimal");
    return 1;

}

int
Message::addParameter(QString name, QByteArray value)
{
    if (parameters.contains(name))
    {
        return false;
    }
    QRegExp rx("^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{4}|[A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)$");
    if (rx.indexIn(QString(value)) == -1)
    {
        QString b64(value.toBase64());
        parameters.insert(name, b64);
    }
    else
    {
        parameters.insert(name, QString(value));
    }

    parameter_types.insert(name, "b64");
    return 1;
}

QDateTime
Message::getTimestamp()
{
    if (timestamp.isNull())
    {
        timestamp = QDateTime::currentDateTime();
    }
    return timestamp;
}

void
Message::setTimestamp(QDateTime ts)
{
    timestamp = ts;
}
