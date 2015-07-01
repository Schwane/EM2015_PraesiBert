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

#include "Message.hpp"
#include "XMLMessageParser.hpp"
#include "XMLMessageWriter.hpp"
#include "ClientSocket.h"
#include "Praesentation.hpp"
#include "commands.hpp"

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

    /* Routines for access from UI */
    Q_INVOKABLE bb::cascades::Image getSlide();
    Q_INVOKABLE QString getLastSentMsg();
    Q_INVOKABLE QString getLoginState();
    Q_INVOKABLE void requestSlideChange(int offset);
    Q_INVOKABLE void sendArbitraryCommand(QString cmd);
    /* Slots to handle signals from socket */
    void onMessageParsed(Message* msg);
    void connectionLost();

    /*Slots to handle praesentation signals*/
    void onPraesiSlideChanged(bb::cascades::Image img);

public:
    Q_INVOKABLE void invokeRemote(Message *msg);
    Q_INVOKABLE void invokeRemote(Message *msg, bool cleanup);
    Q_INVOKABLE void login();
    Q_INVOKABLE void connectToServer(QString addr, QString cmd_port, QString data_port);

Q_SIGNALS:
    void slideChanged();
    void messageSent();
    void loginStateChanged();

protected:
    QMap<QString, remoteFunction> registerdFunctions;
    bb::cascades::Image m_slide;
    XMLMessageParser* xmlmp;
    XMLMessageWriter* xmlmw;
    QString lastSentMsg;
    Network::ClientSocket *cs;
    LoginState login_state;
    Praesentation *prs;
    QString id;

};

#endif /* CLIENT_HPP_ */
