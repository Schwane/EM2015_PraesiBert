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
            TRYING,
            ACCEPTED,
            REJECTED
        };
    Client();
    virtual ~Client();
public Q_SLOTS:
    Message* setSlide(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Message* loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    Q_INVOKABLE bb::cascades::Image getSlide();
    Q_INVOKABLE QString getLastSentMsg();
    Q_INVOKABLE QString getLoginState();
    void onMessageParsed(Message* msg);
    Q_INVOKABLE void simulateSocketSent(QString message);
    void connectionLost();

public:
    Q_INVOKABLE void invokeRemote(Message *msg);
    Q_INVOKABLE void invokeRemote(Message *msg, bool cleanup);
    Q_INVOKABLE void simulateSocketReceive(QString message);
    Q_INVOKABLE void login();
    Q_INVOKABLE void connectToServer(QString addr, QString cmd_port, QString data_port);
    Q_INVOKABLE void requestImage();

Q_SIGNALS:
    void slideChanged();
    void messageSent();
    void loginStateChanged();
protected:
    QMap<QString, remoteFunction> registerdFunctions;
    bb::cascades::Image m_slide;
    XMLMessageParser* xmlmp;
    XMLMessageWriter* xmlmw;
    //DummSocket* ds;
    QString lastSentMsg;
    Network::ClientSocket *cs;
    LoginState login_state;

};

#endif /* CLIENT_HPP_ */
