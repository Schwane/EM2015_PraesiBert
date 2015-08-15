/*
 * Client.hpp
 *
 *  Created on: 22.05.2015
 *      Author: Jan Zimmer
 */

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

/*Qt and BB*/
#include <QObject>
#include <QtGui/QImage>
#include <QtCore>
#include <bb/ImageData>
#include <bb/cascades/Image>
#include <bb/utility/ImageConverter>

/*Self-implemented*/
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

/*!Type of function capable of being executed remotely*/
typedef Message* (Client::*remoteFunction)(QMap<QString, QVariant> parameters, QMap<QString, QString> paramter_types);

//! Basic implementation of Client.
/*!
 * Basic implementation of Client.
 * Contains all members which are included in both Listener and Master.
 */
class Client : public QObject
{
    Q_OBJECT
public:
    //!Loginstate of client.
    enum LoginState
        {
            IDLE,/*!< Client is not connected and not logged in */
            CONNECTING, /*!< Client is connecting to server */
            CONNECTED, /*!< Client is connected to server (only on TCP Layer */
            TRYING, /*!< Client is trying to log in to server */
            ACCEPTED, /*!< Client login was accepted */
            REJECTED /*!< Client login was rejected */
        };
    //! Constructor which initializes all members automatically and connects all signals and slots.
    Client();
    //! Destructor cleans up everything automatically.
    virtual ~Client();
public Q_SLOTS:
    //===============================================================================
    /*Routines for remote execution from serverside */
    //! Execute remote: Set slide to x.
    /*!
     * Reacts to a slide change request.
     * Message should contain slide parameter of type int which indicates number of slide to set the presentation to.
     * Response is either OK or an error message which explains the reason for the error.
     * \param parameters Map of name and parameter value from Message
     * \param parameter_types MAp of name and type of parameter from Message
     * \return Response to receveived command.
     */
    Message* setSlide(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //! Execute remote: Parse presentation from Message.
    /*!
     * Reacts to a parse presentation command.
     * Message should contain following parameters:
     * total_slides - int - number of total slides
     * presentationID - string - id of presentation
     *
     * Response is either OK or an error message which explains the reason for the error.
     * \param parameters Map of name and parameter value from Message
     * \param parameter_types MAp of name and type of parameter from Message
     * \return Response to receveived command.
     */
    Message* parsePraesentation(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //! Execute remote: login response.
    /*!
     * Reacts to a response from server after login try.
     * Message should contain a status of type string with value either "OK" or something else.
     * Response is either OK or an error message which explains the reason for the error.
     * \param parameters Map of name and parameter value from Message
     * \param parameter_types Map of name and type of parameter from Message
     * \return Response to receveived command.
     */
    Message* loginResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //! Execute remote: stop presentation.
   /*!
    * Reacts to a stop request.
    * Repsonse is a ACK.
    * \param parameters Map of name and parameter value from Message
    * \param parameter_types Map of name and type of parameter from Message
    * \return Response to receveived command.
    */
    Message* stopPraesentation(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types);
    //===============================================================================
    /* Routines for access from UI */
    //! Access loginstate as human readable string.
    /*!
     * Access loginstate as human readable string. Ment for GUI access.
     * \return String which is congruent to current loginstate of Client.
     */
    Q_INVOKABLE QString getLoginState();
    //! Access base path of presentation.
    /*!
    * Access base path of presentation. Ment for GUI access (especially for audio recording in QML).
    * \return Basepath of presentation.
    */
    Q_INVOKABLE QString getBasepath();
    //! Request a slide change.
    /*!
     * Request a slide change from extern. For example, a UI Button or the gesture control can access those function either directly or via signal/slot.
     * \param offset relative position of the next slide.
     * \sa reqeustSlideChangeAbsolute
     */
    Q_INVOKABLE void requestSlideChange(int offset);
    //! Request a slide change.
    /*!
     * Request a slide change from extern.
     * Used for direct access to a specific slide.
     * \param slide absolute position of the next slide.
     * \sa requestSlideChange
     */
    Q_INVOKABLE void requestSlideChangeAbsolute(int slide);
    //! Send arbitrary command to server. Implemented only for testpurposes.
    Q_INVOKABLE void sendArbitraryCommand(QString cmd);

    //===============================================================================
    /* Slots to handle signals from socket */
    //° Executed if connection to server is lost (on TCP Layer)
    void connectionLost();


    //===============================================================================
    /*Slots to handle other UI actions*/
    //! Initiate a delivery of an audio file via dataport of networklayer.
    /*!
     * Initiates a delivery of an audio file via the data port of the networklayer-
     * Is primary called after the presentation is stopped or after a talk request is finished.
     * \param path Path to the file which should be delivered to the server.
     */
    Q_INVOKABLE void deliverRecording(QString path);
    /*! Remote procedure call.
     * \param Message from Server.
     * \sa invokeRemote
     */
    Q_INVOKABLE void invokeRemote(Message *msg);
    //! Remote procedure call.
    /*!
     * Existential function in concept of remote procedure call.
     * After Message is received by networklayer it is passed to the xml message parser.
     * There it gets parsed to a message object which is then executed by this function.
     * This function takes the message and looks up the corresponding member function which is ment to execute the remote command by looking it up in member registeredFunctions.
     * Note that this is a whitelist approach; only commands which are registered can be executed, all others are silently dropped.
     * \param msg Message containing the command and payload.
     * \param cleanup flag if the Message should be deleted after processing. Extension point for later versions where Message may be passed further afterwards.
     */
    Q_INVOKABLE void invokeRemote(Message *msg, bool cleanup);
    //! Initiate login to server.
    Q_INVOKABLE void login();
    //! Initiate logout from server.
    Q_INVOKABLE void logout();
    //! Connect to a server (on network layer) with command port and data port on specific IP address.
    /*! Connect to a server (on network layer) with command port and data port on specific IP address.
     * Set loginstate to connecting.
     * \param addr IP address of server
     * \param cmd_port Command port
     * \param data_port Data port
     */
    Q_INVOKABLE void connectToServer(QString addr, QString cmd_port, QString data_port);

    //===============================================================================
    /*HDMI slots*/
    //! Slot for HDMI output if slide changed in presentation.
    /*!
     * Slot for HDMI output if slide changed in presentation. Delivers URL to HDMI class because the HDMI class does not implement signal slot features.
     */
    void onNewSlideUrl(QUrl url);
signals:
    //! Signal is thrown if slide changes and contains the image directly. Wrapper for QML access.
    void slideChanged(bb::cascades::Image img);
    //! Signal is thrown if slide changes and contains URL to new slide. Wrapper for QML access.
    void slideChangedUrl(QUrl url);
    //TODO DELETE
    void messageSent();
    //! Signal is thrown if loginstate has changed.
    void loginStateChanged();
    //! Signal is thrown if longer action is done and UI (or anything else) should indicate waiting state.
    void wait(bool active);
    //! Signal is thrown if a valid presentation is loaded (either remotely (listener) or local (master). Wrapper for QML access.
    void praesentationReady();
    //! Signal is thrown if limits of slides is reached and next slide is accessed. Wrapper for QML access-
    void noMoreSlides();

protected:
    //! Contains mapping from remote command to member function.
    QMap<QString, remoteFunction> registerdFunctions;
    //TODO DELETE
    bb::cascades::Image m_slide;

    //! XML Message parser for commands.
    XMLMessageParser* xmlmp;
    //! XML Message writer to command port.
    XMLMessageWriter* xmlmw;

    //! XML Message parser for data.
    XMLMessageParser* xmlmp_data;
    //! XML Message writer to data port.
    XMLMessageWriter* xmlmw_data;

    //! Instance of networklayer.
    Network::ClientSocket *cs;
    //! Loginstate of the client.
    LoginState login_state;
    //! ID of the client.
    /*!
     * ID of client.
     * If not logged in, it is set to "undefined_client"
     * After login it is set to "master" for Master and id received from server for listener.
     */
    QString id;
    //! Instance of presentation.s
    Praesentation *prs;

    //! Instance of HDMI output wrapper..
    bb::EM2015::HDMI *hdmi;
};

#endif /* CLIENT_HPP_ */
