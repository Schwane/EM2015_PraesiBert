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
    registerdFunctions.insert(CMD_STOP_PRAESENTATION,&Client::stopPraesentation);
    registerdFunctions.insert(CMD_SET_PRAESENTATION,&Client::parsePraesentation);
    registerdFunctions.insert(CMD_LOGIN_RESP, &Client::Client::loginResponse);

    xmlmp = new XMLMessageParser();
    xmlmw = new XMLMessageWriter();

    xmlmp_data = new XMLMessageParser();
    xmlmw_data = new XMLMessageWriter();


    cs = new Network::ClientSocket(this);
    prs = new Praesentation();

    id = "undefined_client";

    login_state = IDLE;

    connect(cs,SIGNAL(receivedCmd(QByteArray)),xmlmp,SLOT(parseMessage(QByteArray)));
    connect(cs,SIGNAL(receivedData(QByteArray)),xmlmp_data,SLOT(parseMessage(QByteArray)));

    connect(xmlmp,SIGNAL(messageParsed(Message*)),this,SLOT(invokeRemote(Message*)));
    connect(xmlmw,SIGNAL(messageWritten(QByteArray)),cs,SLOT(sendCmd(QByteArray)));

    connect(xmlmp_data,SIGNAL(messageParsed(Message*)),this,SLOT(invokeRemote(Message*)));
    connect(xmlmw_data,SIGNAL(messageWritten(QByteArray)),cs,SLOT(sendData(QByteArray)));

    connect(cs, SIGNAL(connectedToCmdServer()),this,SLOT(login()));
    connect(cs, SIGNAL(lostConnection()),this,SLOT(connectionLost()));

    connect(prs, SIGNAL(slideChanged(bb::cascades::Image)), this, SIGNAL(slideChanged(bb::cascades::Image)));
    connect(prs, SIGNAL(slideChangedUrl(QUrl)), this, SIGNAL(slideChangedUrl(QUrl)));
    connect(prs, SIGNAL(slideChangedUrl(QUrl)), this, SLOT(onNewSlideUrl(QUrl)));
    connect(prs, SIGNAL(parsing(bool)), this, SIGNAL(wait(bool)));
    connect(prs, SIGNAL(praesentationReady()), this, SIGNAL(praesentationReady()));

    hdmi = new bb::EM2015::HDMI(RES1280x720);
}

Client::~Client()
{
    delete hdmi;
    delete xmlmp;
    delete xmlmw;

    delete xmlmp_data;
    delete xmlmw_data;

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

    if (slide < 0)
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: slide - smaller than 0"));
        return resp;
    }

    if (prs->getTotalSlides() <= slide)
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: slide - bigger than total slides"));
        emit noMoreSlides();
        return resp;
    }

    prs->setSlide(slide);
    resp -> addParameter("status", QString("ok"));

    return resp;
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
Client::stopPraesentation(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    prs->stop();
    //bb::ImageData imgData = bb::utility::ImageConverter::decode(QUrl("assets:///img/before_start.png"));
    //bb::cascades::Image img = imgData;
    //emit slideChanged(img);
    emit slideChangedUrl(QUrl("asset:///img/before_start.png"));
    hdmi->show_slide(QUrl("asset:///img/before_start.png"));
    Message *msg = new Message(CMD_ACK_RESPONSE, id, "server");
    return msg;
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
/*
void
Client::onMessageParsed(Message* msg)
{
    invokeRemote(msg);
}
*/

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

/*
void
Client::onPraesiSlideChanged(bb::cascades::Image img)
{
    m_slide = img;
    emit slideChanged();
}
*/

void
Client::requestSlideChange(int offset)
{
    Message *msg = new Message(CMD_SET_SLIDE, id, "server");
    msg->addParameter("slide", prs->getCurrentSlide() + offset);
    xmlmw->writeMessage(msg);
}

void
Client::requestSlideChangeAbsolute(int slide)
{
    Message *msg = new Message(CMD_SET_SLIDE, id, "server");
    msg->addParameter("slide", slide);
    xmlmw->writeMessage(msg);
}

void
Client::sendArbitraryCommand(QString cmd)
{
    Message *msg = new Message(cmd, id, "server");
    xmlmw->writeMessage(msg);
}

/*
void
Client::onRunning(bool active)
{
    if (active)
    {
       ar_path = ar.record();
    }
    else
    {
       ar_length = ar.stop();
       deliverRecording();
    }

}
*/

void
Client::deliverRecording(QString path)
{
    Message *msg = new Message(DATA_AUDIO, id, "server");
    path = path.replace("file://", "");
    QFile recording(path);

    if(!recording.open(QIODevice::ReadOnly))
    {
        delete msg;
        return;
    }
    QByteArray content;
    content = recording.readAll();

    msg->addParameter("audio", content);
    xmlmw_data -> writeMessage(msg);
}

void
Client::logout()
{
    cs -> disconnectFromServer();
}

void
Client::onNewSlideUrl(QUrl url)
{
    hdmi -> show_slide(url);
}

QString
Client::getBasepath()
{
    return prs->getBasepath();
}
