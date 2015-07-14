/*
 * Client.hpp
 *
 *  Created on: 22.05.2015
 *      Author: JCB
 */

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <QObject>
#include <QtGui/QImage>
#include <QtCore>
#include <bb/ImageData>
#include <bb/cascades/Image>
#include <bb/utility/ImageConverter>

#include "Message.hpp"
#include "XMLMessageParser.hpp"
#include "XMLMessageWriter.hpp"
#include "ClientSocket.h"
#include "Praesentation.hpp"
#include "commands.hpp"
#include "EMaudiorecorder.hpp"
#include "HDMI.hpp"

class Client;
class XMLMessageParser;

typedef Message* (Client::*remoteFunction)(QMap<QString, QVariant> parameters, QMap<QString, QString> paramter_types);

class Client : public QObject
{
    Q_OBJECT

public:
    enum LoginState
        {
            IDLE,
            CONNECTING,
            CONNECTED,
            TRYING,
            ACCEPTED,
            REJECTED
        };
    Client();
    virtual ~Client();
public Q_SLOTS:
    /*Routines for remote execution from serverside */
    Message* setSlide(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Message* parsePraesentation(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Message* loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Message* stopPraesentation(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    /* Routines for access from UI */
    Q_INVOKABLE bb::cascades::Image getSlide();
    Q_INVOKABLE QString getLastSentMsg();
    Q_INVOKABLE QString getLoginState();
    Q_INVOKABLE QString getBasepath();
    Q_INVOKABLE void requestSlideChange(int offset);
    Q_INVOKABLE void requestSlideChangeAbsolute(int slide);
    Q_INVOKABLE void sendArbitraryCommand(QString cmd);
    /* Slots to handle signals from socket */
    //void onMessageParsed(Message* msg);
    void connectionLost();

    /*Slots to handle praesentation signals*/
    Q_INVOKABLE void deliverRecording(QString path);

    //void onPraesiSlideChanged(bb::cascades::Image img);
    Q_INVOKABLE void invokeRemote(Message *msg);
    Q_INVOKABLE void invokeRemote(Message *msg, bool cleanup);
    Q_INVOKABLE void login();
    Q_INVOKABLE void logout();
    Q_INVOKABLE void connectToServer(QString addr, QString cmd_port, QString data_port);

    /*HDMI slots*/
    void onNewSlideUrl(QUrl url);
signals:
    void slideChanged(bb::cascades::Image img);
    void slideChangedUrl(QUrl url);
    void messageSent();
    void loginStateChanged();
    void wait(bool active);
    void praesentationReady();
    void noMoreSlides();

protected:
    QMap<QString, remoteFunction> registerdFunctions;
    bb::cascades::Image m_slide;

    XMLMessageParser* xmlmp;
    XMLMessageWriter* xmlmw;

    XMLMessageParser* xmlmp_data;
    XMLMessageWriter* xmlmw_data;

    QString lastSentMsg;
    Network::ClientSocket *cs;
    LoginState login_state;
    Praesentation *prs;
    QString id;

    bb::EM2015::HDMI *hdmi;
};

#endif /* CLIENT_HPP_ */
