/*
 * XMLMessageParser.cpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#include "include/XMLMessageParser.hpp"

XMLMessageParser::XMLMessageParser()
{

}

XMLMessageParser::~XMLMessageParser()
{
}

void
XMLMessageParser::parseMessage(QByteArray bytes)
{
    emit messageParsed(messageParser(bytes));
}
void
XMLMessageParser::parseCmdMessage(QByteArray bytes)
{
    emit cmdMessageParsed(messageParser(bytes));
}

void
XMLMessageParser::parseDataMessage(QByteArray bytes)
{
    emit dataMessageParsed(messageParser(bytes));
}

void
XMLMessageParser::parseCmdMessage(QByteArray bytes, uint clientId)
{
    emit cmdMessageParsed(messageParser(bytes), clientId);
}

void
XMLMessageParser::parseDataMessage(QByteArray bytes, uint clientId)
{
    emit dataMessageParsed(messageParser(bytes), clientId);
}

Message*
XMLMessageParser::messageParser(QByteArray& bytes)
{
    //Xml stream
    QXmlStreamReader *xmlr = new QXmlStreamReader(bytes);
    QString command, sender, receiver, parameter, type, data, name;

    //Flags for fields
    bool inHeader, inParameters, inParameter;
    bool okConv, okAdd;
    QDateTime dataDate;
    QByteArray dataBytes;

    QXmlStreamAttributes attrs;

    //Initial message
    Message *msg = new Message("not set", "not set", "not set");

    inHeader = false;
    inParameters = false;

    okConv = false;
    okAdd = false;

    xmlr->readNext();

    while (!xmlr->isEndDocument() && !xmlr->hasError()) //For whole XML
    {
        if (xmlr->isStartElement())
        {
            name = xmlr->name().toString();
            if ( name ==  "header"){
                inHeader = true; //if in header
            }
            else if ( name ==  "sender"){
                if (inHeader) //sender is always in header
                {
                    msg -> sender = xmlr->readElementText().trimmed(); //get \wo whitespaces
                }
            }
            else if ( name ==  "receiver"){
                if (inHeader) //receiver is always in header
                {
                    msg -> receiver = xmlr->readElementText().trimmed();
                }
            }
            else if ( name == "timestamp"){
                if (inHeader)//time stamp is always in header
                {
                    msg -> timestamp = QDateTime::fromString(xmlr->readElementText().trimmed(), MESSAGE_DATETIME_FORMAT);
                }
            }
            else if ( name ==  "command"){
                msg -> command = xmlr->readElementText().trimmed();
            }
            else if ( name ==  "parameters"){
                inParameters = true;
                }
            else if ( name ==  "parameter"){
                if (inParameters)
                {
                    attrs = xmlr->attributes();
                    //paremters must have namer/type
                    if (attrs.hasAttribute("name") && attrs.hasAttribute("type"))
                    {
                       parameter = attrs.value("name").toString(); //set name and type for later
                       type = attrs.value("type").toString();
                    }
                    inParameter = true;
                }
            }
            else if ( name ==  "data"){
                if (inParameter && inParameters)
                {
                    data = xmlr->readElementText().trimmed();
                    //parse allowed types and convert data by adding it to the message object
                    if (type == "integer"){
                            okAdd = msg->addParameter(parameter, data.toInt(&okConv, 10));
                    }
                    else if (type == "decimal"){
                            okAdd = msg->addParameter(parameter, data.toDouble(&okConv));
                    }
                    else if ( type ==  "b64"){
                        dataBytes.clear();
                        dataBytes.append(data);
                        okAdd = msg ->addParameter(parameter, dataBytes);
                    }
                    else if ( type ==  "date"){
                        dataDate = QDateTime::fromString(data, MESSAGE_DATETIME_FORMAT);
                        okAdd = msg ->addParameter(parameter, dataDate);
                    }
                    else if ( type ==  "string"){
                        okAdd = msg ->addParameter(parameter, data);
                    }
                    else{
                        okAdd = false;
                        okConv = false;
                    }
                }
            }
        }
        else if (xmlr->isEndElement())
        {
            name = xmlr->name().toString();

            //reset flags on close tag
            if ( name ==  "header"){
                inHeader = false;
            }
            else if ( name ==  "parameters"){
                inParameters = false;
            }
            else if ( name ==  "parameter"){
                inParameter = true;
            }
        }

        xmlr->readNext();
    }

    //For now, errors are silently ignored
    if (xmlr->hasError())
    {
        qDebug() << "XML error: " << xmlr->errorString().data();
    }

    delete xmlr;

    return msg;
}
