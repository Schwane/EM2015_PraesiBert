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
    registerdFunctions.insert(CMD_SET_SLIDE,&Client::setSlide);
    registerdFunctions.insert(CMD_SET_PRAESENTATION,&Client::parsePraesentation);

    xmlmp = new XMLMessageParser();
    xmlmw = new XMLMessageWriter();
    cs = new Network::ClientSocket(this);
    prs = new Praesentation();

    id = "undefined_client";

    login_state = IDLE;

    connect(cs,SIGNAL(receivedCmd(QByteArray)),xmlmp,SLOT(parseMessage(QByteArray)));
    connect(xmlmp,SIGNAL(messageParsed(Message*)),this,SLOT(onMessageParsed(Message*)));
    connect(xmlmw,SIGNAL(messageWritten(QByteArray)),cs,SLOT(sendCmd(QByteArray)));
    connect(cs, SIGNAL(connectedToCmdServer()),this,SLOT(login()));
    connect(cs, SIGNAL(lostConnection()),this,SLOT(connectionLost()));
}

Client::~Client()
{
    delete xmlmp;
    delete xmlmw;
    delete prs;
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
    Message *resp = new Message(CMD_ACK_RESPONSE, id,"server");

    if(!parameters.contains("slide"))
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: slide number not defined"));
        return resp;
    }

    if(!parameter_types.contains("slide") || parameter_types.value("slide") != "integer")
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: slide - WRONG TYPE"));
        return resp;
    }

    int slide = parameters.value("slide").toInt();

    if (prs->getTotalSlides() < slide)
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: slide - bigger than total slides"));
        return resp;
    }

    prs->setSlide(slide);
    resp -> addParameter("status", QString("ok"));

    return resp;
    /*
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
    */
}

Message*
Client::parsePraesentation(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp = new Message(CMD_ACK_RESPONSE, id, "server");
    resp->addParameter("status", QString("ok"));

    prs->parsePraesentation(parameters, parameter_types);
    //TODO correct response needs to be generated! This is only a quick-fix (Sebastian).
    return resp;
}

Message*
Client::loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp = new Message(CMD_ACK_RESPONSE,"client","server");

    if (parameters.contains("status") && parameters.contains("id"))
    {
        QString status = parameters.value("status").toString();
        QString id = parameters.value("id").toString();
        if (status == "ok")
        {
            login_state = ACCEPTED;
            this -> id = id;
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
    if (login_state == CONNECTED)
    {
        return QString("connected");
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
    login_state = CONNECTED;
    emit loginStateChanged();
    Message* msg = new Message("login", id, "server");
    xmlmw -> writeMessage(msg);
    login_state = TRYING;
    emit loginStateChanged();
}

void
Client::connectionLost()
{
    login_state = IDLE;
    emit loginStateChanged();
}

void
Client::onPraesiSlideChanged(bb::cascades::Image img)
{
    m_slide = img;
    emit slideChanged();
}

void
Client::requestSlideChange(int offset)
{
    Message *msg = new Message("setSlide", id, "server");
    msg->addParameter("slide", prs->getCurrentSlide() + offset);
    xmlmw->writeMessage(msg);
}

void
Client::sendArbitraryCommand(QString cmd)
{
    Message *msg = new Message(cmd, id, "server");
    xmlmw->writeMessage(msg);
}
