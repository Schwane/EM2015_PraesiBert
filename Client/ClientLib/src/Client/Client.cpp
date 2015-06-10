/*
 * Client.cpp
 *
 *  Created on: 22.05.2015
 *      Author: JCB
 */

#include "include/Client.hpp"


Client::Client()
{
    /*register remote functions*/
    registerdFunctions.insert("slide",&Client::setSlide);
    //registerdFunctions.insert("login",&Client::loginResponse);

    xmlmp = new XMLMessageParser();
    xmlmw = new XMLMessageWriter();
    //ds = new DummSocket();
    cs = new Network::ClientSocket(this);

    //connectToServer("192.168.1.144", "2000");

    //connect(ds,SIGNAL(received(QByteArray)),xmlmp,SLOT(parseMessage(QByteArray)));
    connect(cs,SIGNAL(newData(QByteArray)),xmlmp,SLOT(parseMessage(QByteArray)));
    connect(xmlmp,SIGNAL(messageParsed(Message*)),this,SLOT(onMessageParsed(Message*)));
    //connect(xmlmw,SIGNAL(messageWritten(QByteArray)),ds,SLOT(send(QByteArray)));
    connect(xmlmw,SIGNAL(messageWritten(QByteArray)),cs,SLOT(sendData(QByteArray)));
    //connect(ds,SIGNAL(sent(QString)),this,SLOT(simulateSocketSent(QString)));
    connect(cs, SIGNAL(connected()),this,SLOT(login()));
    connect(cs, SIGNAL(disconnected()),this,SLOT(connectionLost()));
}

Client::~Client()
{
    delete xmlmp;
    //delete ds;
}

void
Client::invokeRemote(Message *msg)
{
    invokeRemote(msg, true);
}

void Client::invokeRemote(Message *msg, bool cleanup)
{
    qDebug() << "Invoke";
    QString cmd(msg -> command);
    qDebug() << "check if function is contained";

    Message *response;

    if((registerdFunctions.contains(cmd)))
    {
        remoteFunction rmt;
        rmt = registerdFunctions.value(cmd);
        response = (this->*rmt)(msg -> parameters, msg -> parameter_types);
        xmlmw -> writeMessage(response);
    }
    if(cleanup)
    {
        delete msg;
    }
}

bb::cascades::Image
Client::getSlide()
{
    return m_slide;
}

Message*
Client::setSlide(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp = new Message("RESPONSE","cl","gui");

    if(!parameters.contains("image"))
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: image - NOT DEFINED"));
        return resp;
    }

    if(!parameter_types.contains("image") || parameter_types.value("image") != "b64")
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: image - WRONG TYPE"));
        return resp;
    }
    QVariant imgVar = parameters.value("image");
    QByteArray imgBytes;
    imgVar.convert(QVariant::String);
    imgBytes.append(imgVar.toString());
    imgBytes = QByteArray::fromBase64(imgBytes);

    QImage img;

    img = QImage::fromData(imgBytes, "JPG");

    QImage swappedImage = img.rgbSwapped();
    if(swappedImage.format() != QImage::Format_RGB32) {
        swappedImage = swappedImage.convertToFormat(QImage::Format_RGB32);
    }

    const bb::ImageData imgData= bb::ImageData::fromPixels(swappedImage.bits(), bb::PixelFormat::RGBX, swappedImage.width(), swappedImage.height(), swappedImage.bytesPerLine());

    if (imgData.isValid())
    {
        this->m_slide = bb::cascades::Image(imgData);
        emit slideChanged();
        resp -> addParameter("status", QString("ok"));
    }
    else
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: image - ImageData conversion failed"));
    }

    return resp;
}

Message*
Client::loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp = new Message("RESPONSE","client","server");

    if (parameters.contains("status"))
    {
        QString status = parameters.value("status").toString();
        if (status == "ok")
        {
            login_state = ACCEPTED;
        }
        else
        {
            login_state = REJECTED;
        }
        emit loginStateChanged();
        resp -> addParameter("status", QString("ok"));
    }
    else
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: status - no status contained"));
    }

    return resp;
}


void
Client::simulateSocketReceive(QString message)
{
    QByteArray bytes;
    bytes.append(message);
    //ds->receive(bytes);
}


void
Client::simulateSocketSent(QString message)
{
    lastSentMsg = message;
    emit messageSent();
}

void
Client::onMessageParsed(Message* msg)
{
    invokeRemote(msg);
}

QString
Client::getLastSentMsg()
{
    return lastSentMsg;
}

QString
Client::getLoginState()
{
    if (login_state == TRYING)
    {
        return QString("trying...");
    }
    if (login_state == ACCEPTED)
    {
        return QString("accepted");
    }
    if (login_state == CONNECTING)
    {
        return QString("connecting...");
    }
    if (login_state == IDLE)
    {
        return QString("not connected");
    }
    if (login_state == REJECTED)
    {
        return QString("rejected");
    }
    return QString("unknown");
}


void
Client::connectToServer(QString addr, QString cmd_port, QString data_port)
{
    if (login_state != ACCEPTED)
    {
        cs -> connectToServer(addr, cmd_port, data_port);
        login_state = CONNECTING;
        emit loginStateChanged();
    }
}

void
Client::login()
{
    Message* msg = new Message("login", "client", "server");
    xmlmw -> writeMessage(msg);
    login_state = TRYING;
    emit loginStateChanged();
}

void
Client::requestImage()
{
    Message* msg = new Message("test", "client", "server");
    xmlmw -> writeMessage(msg);
}

void
Client::connectionLost()
{
    login_state = IDLE;
    emit loginStateChanged();
}
