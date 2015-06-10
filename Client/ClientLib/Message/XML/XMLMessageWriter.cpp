/*
 * XMLMessageWriter.cpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#include "XMLMessageWriter.hpp"

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



void
XMLMessageWriter::writeMessage(Message* msg, bool cleanup)
{
    QByteArray msgBytes;

    QXmlStreamWriter xmlw(&msgBytes);

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



    if (cleanup)
    {
        delete msg;
    }
    emit messageWritten(msgBytes);
}

void
XMLMessageWriter::writeMessage(Message* msg)
{
    writeMessage(msg, true);
}
