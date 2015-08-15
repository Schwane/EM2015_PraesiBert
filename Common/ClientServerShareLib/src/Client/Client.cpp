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

    /* create XML objects for both data and command port */
    xmlmp = new XMLMessageParser();
    xmlmw = new XMLMessageWriter();

    xmlmp_data = new XMLMessageParser();
    xmlmw_data = new XMLMessageWriter();

    /* create network layer */
    cs = new Network::ClientSocket(this);
    prs = new Praesentation();

    id = "undefined_client"; // ID is undefined until log in to server

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

    //Instance of HDMI display for all clients
    hdmi = new bb::EM2015::HDMI(RES1280x720);
}

Client::~Client()
{
    //clean up
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
    //invoke with clean up by default
    invokeRemote(msg, true);
}

void Client::invokeRemote(Message *msg, bool cleanup)
{
    QString cmd(msg -> command);

    Message *response;

    //whitelisting by command
    if((registerdFunctions.contains(cmd)))
    {
        remoteFunction rmt;
        rmt = registerdFunctions.value(cmd);
        //Execute remote function with parameters from message object
        response = (this->*rmt)(msg -> parameters, msg -> parameter_types); //Note that a repsonse should always be generated either as error or as ack or containing data for the next step.
        xmlmw -> writeMessage(response);
    }
    //only clean up if needed
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
    //Check if all parameters are contained and send error if not.
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

    //Check boundaries of slide and either set slide or emit error.
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
    //Parse presentation from message
    prs->parsePraesentation(parameters, parameter_types);

    return resp;
}

Message*
Client::stopPraesentation(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    //Stop presentation and emit default slide which should be shown at stop.
    prs->stop();
    emit slideChangedUrl(QUrl("asset:///img/before_start.png"));
    hdmi->show_slide(QUrl("asset:///img/before_start.png"));
    Message *msg = new Message(CMD_ACK_RESPONSE, id, "server");
    return msg;
}

Message*
Client::loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *resp = new Message(CMD_ACK_RESPONSE,"client","server");
    //Check if parameters are contained
    if (parameters.contains("status") && parameters.contains("id"))
    {
        QString status = parameters.value("status").toString();
        QString id = parameters.value("id").toString();
        if (status == "ok") //if status is ok then login is done and client id is set to that from server
        {
            login_state = ACCEPTED;
            this -> id = id;
        }
        else
        {
            login_state = REJECTED;
        }
        emit loginStateChanged(); //inform that state has changed
        resp -> addParameter("status", QString("ok"));
    }
    else //error
    {
        resp -> addParameter("status",QString("error"));
        resp -> addParameter("message",QString("Parameter: status - no status contained"));
    }

    return resp;
}



QString
Client::getLoginState()
{
    //Return human readable string of current state
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
    //Only connect if not already connected
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
    //after connection (on tcp layer) we attempt a login
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
    //if connection lost, set to idle
    login_state = IDLE;
    emit loginStateChanged();
}

void
Client::requestSlideChange(int offset)
{
    //request the server to set slide to given offset with respect to current slide
    Message *msg = new Message(CMD_SET_SLIDE, id, "server");
    msg->addParameter("slide", prs->getCurrentSlide() + offset);
    xmlmw->writeMessage(msg);
}

void
Client::requestSlideChangeAbsolute(int slide)
{
    //request server to set slide to give slide no
    Message *msg = new Message(CMD_SET_SLIDE, id, "server");
    msg->addParameter("slide", slide);
    xmlmw->writeMessage(msg);
}

void
Client::sendArbitraryCommand(QString cmd)
{
    //only for debug purposes, send arbitrary command to server.
    Message *msg = new Message(cmd, id, "server");
    xmlmw->writeMessage(msg);
}

void
Client::deliverRecording(QString path)
{
    //with give path ...
    Message *msg = new Message(DATA_AUDIO, id, "server");
    path = path.replace("file://", "");
    QFile recording(path);

    //.. try to open, ...
    if(!recording.open(QIODevice::ReadOnly))
    {
        delete msg;
        return;
    }
    QByteArray content;
    // ... read content of audio file ...
    content = recording.readAll();

    msg->addParameter("audio", content);
    //... and send it to the server.
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
    //Because hdmi doesnt have slots we have to wrap the functions here.
    hdmi -> show_slide(url);
}

QString
Client::getBasepath()
{
    //Wrapper for access from QML.
    return prs->getBasepath();
}
