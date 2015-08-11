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
//! Implementation of a Message.
/*!
 * Message:
 * Contains Header:
 * - Sender
 * - Receiver
 * - Timestampt
 * - Command
 * Contains Payload:
 * - Parameters
 * - Parameter types
 *
 * Message is instantiated if needed and finally passed to a XML Writer which serializes the Message to XML and passes it to the network layer.^
 * Message is instantiated by XML Parser if network layer passed a valid XML Message to it.
 *
 * A parameter can be added by calling addParameter with the name of the parameter and the payload.
 * Message automatically recognizes the type of the parameter and adds the type to the parameter type map.
 * For now following types are supported:
 * - integer
 * - decimal
 * - string
 * - date/time
 * - raw byte array (is converted to base64 automatically; has to be converted from base64 _manually_)
 *
 * Member can either be accesd from friend classes or by getters and setter (payload is only accessible by friendclasses).
 * If timestamp is requested and no timestamp is set yet the timestamp will be set to the time it is requested.
 *
 */
class Message : public QObject
{
    Q_OBJECT
    friend class XMLMessageParser;
    friend class XMLMessageWriter;
    friend class Client;
    friend class Praesentation;
public:
    Message();
    Message(QString command, QString sender, QString receiver);
    virtual ~Message();
    QString getCommand();
    QString getSender();
    QString getReceiver();
    const QMap<QString, QVariant>* getParameters();
    const QMap<QString, QString>* getParameterTypes();
    void setParameterList(QMap<QString, QVariant> list);
    void setParameterTypeList(QMap<QString, QString> types);

private:
    //! Command of message.
    QString command;
    //! send of message
    QString sender;
    //! receiver of message
    QString receiver;
    //! Payload
    QMap<QString, QVariant> parameters;
    //! Payload types
    QMap<QString, QString> parameter_types;
    //! Timestamp
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
