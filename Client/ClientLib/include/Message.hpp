/*
 * Message.hpp
 *
 *  Created on: 22.05.2015
 *      Author: JCB
 */

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <QDateTime>
#include <QObject>
#include <QRegExp>
#include "XMLMessageWriter.hpp"


class XMLMessageWriter;
class XMLMessageParser;

#define MESSAGE_DATETIME_FORMAT "dd.MM.yyyy hh:mm:ss.zzz"

class Message : public QObject
{
    Q_OBJECT
    friend class XMLMessageParser;
    friend class XMLMessageWriter;
    friend class Client;
public:
    Message();
    Message(QString command, QString sender, QString receiver);
    virtual ~Message();
private:
    QString command;
    QString sender;
    QString receiver;
    QMap<QString, QVariant> parameters;
    QMap<QString, QString> parameter_types;
    QDateTime timestamp;
public:
    int addParameter(QString name, QString value);
    int addParameter(QString name, QDateTime value);
    int addParameter(QString name, int value);
    int addParameter(QString name, double value);
    int addParameter(QString name, QByteArray value);
    QDateTime getTimestamp();
    void setTimestamp(QDateTime ts);
};

#endif /* MESSAGE_HPP_ */
