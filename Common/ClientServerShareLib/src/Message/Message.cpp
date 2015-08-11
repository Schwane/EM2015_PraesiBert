/*
 * Message.cpp
 *
 *  Created on: 22.05.2015
 *      Author: JCB
 */

#include "include/Message.hpp"

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
}

void
Message::setParameterList(QMap<QString, QVariant> list)
{
    this->parameters = list;
}

void
Message::setParameterTypeList(QMap<QString, QString> types)
{
    this->parameter_types = types;
}

int
Message::addParameter(QString name, QString value)
{
    //Adding a paramter is (nearly) always the same.
    // - first check if the paremter is not already contained.
    // - second add the paremeter payload and type respectively.
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
    //slightly different.
    //Check if data is b64 encoded already with reg exp.
    QRegExp rx("^([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{4}|[A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)$");
    if (rx.indexIn(QString(value)) == -1) //Only convert to b64 if data is not b64.
    {
        QString b64(value.toBase64());
        parameters.insert(name, b64);
    }
    else
    {
        parameters.insert(name, QString(value));
    }
    //at this point we can be sure that data is base 64 encoded
    parameter_types.insert(name, "b64");
    return 1;
}

QDateTime
Message::getTimestamp()
{
    //If timestamp is requested but not stamped, stamp it now!
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

QString Message::getCommand()
{
    return command;
}

QString Message::getSender()
{
    return sender;
}

QString Message::getReceiver()
{
    return receiver;
}

const QMap<QString, QVariant>* Message::getParameters()
{
    return &parameters;
}

const QMap<QString, QString>* Message::getParameterTypes()
{
    return &parameter_types;
}
