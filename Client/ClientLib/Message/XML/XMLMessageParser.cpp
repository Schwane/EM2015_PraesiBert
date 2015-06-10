/*
 * XMLMessageParser.cpp
 *
 *  Created on: 01.06.2015
 *      Author: JCB
 */

#include "XMLMessageParser.hpp"

XMLMessageParser::XMLMessageParser()
{
    //xmlr = QXmlStreamReader();
    //cl = NULL;
}
/*
XMLMessageParser::XMLMessageParser(Client *cl)
{
    this -> cl = cl;
    //xmlr = QXmlStreamReader();
}
*/
XMLMessageParser::~XMLMessageParser()
{
}

void
XMLMessageParser::parseMessage(QByteArray bytes)
{
    QXmlStreamReader *xmlr = new QXmlStreamReader(bytes);
    QString command, sender, receiver, parameter, type, data, name;

    bool inHeader, inParameters, inParameter;
    bool okConv, okAdd;
    QDateTime dataDate;
    QByteArray dataBytes;

    QXmlStreamAttributes attrs;

    Message *msg = new Message("not set", "not set", "not set");

    inHeader = false;
    inParameters = false;

    okConv = false;
    okAdd = false;

    //xmlr.clear();
    //xmlr.addData(bytes);
    xmlr->readNext();

    while (!xmlr->isEndDocument() && !xmlr->hasError())
    {
        if (xmlr->isStartElement())
        {
            name = xmlr->name().toString();
            if ( name ==  "header"){
                inHeader = true;
            }
            else if ( name ==  "sender"){
                if (inHeader)
                {
                    msg -> sender = xmlr->readElementText().trimmed();
                }
            }
            else if ( name ==  "receiver"){
                if (inHeader)
                {
                    msg -> receiver = xmlr->readElementText().trimmed();
                }
            }
            else if ( name == "timestamp"){
                if (inHeader)
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
                    if (attrs.hasAttribute("name") && attrs.hasAttribute("type"))
                    {
                       parameter = attrs.value("name").toString();
                       type = attrs.value("type").toString();
                    }
                    inParameter = true;
                }
            }
            else if ( name ==  "data"){
                if (inParameter && inParameters)
                {
                    data = xmlr->readElementText().trimmed();
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

    if (xmlr->hasError())
    {
        qDebug() << "XML error: " << xmlr->errorString().data();
    }

    emit messageParsed(msg);
    delete xmlr;
}
