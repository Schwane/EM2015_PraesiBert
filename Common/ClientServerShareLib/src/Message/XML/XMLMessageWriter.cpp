/*
 * XMLMessageWriter.cpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#include "include/XMLMessageWriter.hpp"

XMLMessageWriter::XMLMessageWriter()
{
    // TODO Auto-generated constructor stub
}

/*
XMLMessageWriter::XMLMessageParser(Client *cl)
{
}
*/

XMLMessageWriter::~XMLMessageWriter()
{
    // TODO Auto-generated destructor stub
}

/**
 * @brief This function writes a given message into a given QByteArray.
 *
 * ... detailed ...
 *
 * @param[in] msg Pointer to message-object that shall be written into byte-stream (object will not be deleted!).
 * @param[out] outputMessageBytes Pointer to QByteArray-object into which the bytes shall be written.
 */
void
XMLMessageWriter::createMessageByteArray(Message* msg, QByteArray* outputMessageBytes)
{
    QXmlStreamWriter xmlw(outputMessageBytes);

    xmlw.writeStartElement("message");

    xmlw.writeStartElement("header");
    xmlw.writeTextElement("sender",msg->sender);
    xmlw.writeTextElement("receiver",msg->receiver);
    xmlw.writeTextElement("timestamp",msg->getTimestamp().toString(MESSAGE_DATETIME_FORMAT));
    xmlw.writeEndElement();

    xmlw.writeTextElement("command", msg->command);

    xmlw.writeStartElement("parameters");

    for(QVariantMap::const_iterator iter = (msg->parameters).begin(); iter != (msg->parameters).end(); ++iter) {
      qDebug() << iter.key() << iter.value();
      QString data;
      QString type((msg->parameter_types).value(iter.key()));

      if (type == "date")
      {
          data = iter.value().toDateTime().toString(MESSAGE_DATETIME_FORMAT);
      }
      else
      {
          data = iter.value().toString();
      }

      xmlw.writeStartElement("parameter");
      xmlw.writeAttribute("name",iter.key());
      xmlw.writeAttribute("type",type);
      xmlw.writeTextElement("data",data);
      xmlw.writeEndElement();
    }

    xmlw.writeEndElement();
    xmlw.writeEndElement();
}

void
XMLMessageWriter::writeMessage(Message* msg)
{
    QByteArray msgBytes;

    createMessageByteArray(msg, &msgBytes);
    delete(msg);
    emit messageWritten(msgBytes);
}

void
XMLMessageWriter::writeCmdMessage(Message* msg)
{
    QByteArray msgBytes;

    createMessageByteArray(msg, &msgBytes);
    delete(msg);
    emit cmdMessageWritten(msgBytes);
}

void
XMLMessageWriter::writeDataMessage(Message* msg)
{
    QByteArray msgBytes;

    createMessageByteArray(msg, &msgBytes);
    delete(msg);
    emit dataMessageWritten(msgBytes);
}

void
XMLMessageWriter::writeCmdMessage(Message* msg, uint clientId)
{
    QByteArray msgBytes;

    createMessageByteArray(msg, &msgBytes);
    qDebug() << "Message Writer wirte command: " << msg->getCommand();
    delete(msg);
    emit cmdMessageWritten(msgBytes, clientId);
}

void
XMLMessageWriter::writeDataMessage(Message* msg, uint clientId)
{
    QByteArray msgBytes;

    createMessageByteArray(msg, &msgBytes);
    delete(msg);
    emit dataMessageWritten(msgBytes, clientId);
}

void XMLMessageWriter::writeCmdMessage(Message* msg, QList<uint> clientIDs)
{
    QByteArray msgBytes;

    createMessageByteArray(msg, &msgBytes);
    delete(msg);
    emit cmdMessageWritten(msgBytes, clientIDs);

}

void XMLMessageWriter::writeDataMessage(Message* msg, QList<uint> clientIDs)
{
    QByteArray msgBytes;

    createMessageByteArray(msg, &msgBytes);
    delete(msg);
    emit dataMessageWritten(msgBytes, clientIDs);
}
