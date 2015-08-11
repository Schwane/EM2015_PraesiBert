/*
 * Praesentation.hpp
 *
 *  Created on: 11.06.2015
 *      Author: JCB
 */

#ifndef PRAESENTATION_HPP_
#define PRAESENTATION_HPP_

#include <QtCore>
#include <QObject>
#include <bb/cascades/Image>
#include <bb/utility/ImageConverter>
#include <bb/ImageData>

#include "Message.hpp"
#include "commands.hpp"
//! Class to hold information about the presentation.
/*!
 * This class hold the information about the current status of the presentation.
 * It holds the URLs to the slides, the number of total slides and the current slide, as well as an identifier.
 * It is implemented thread safe to enable access from multiple threads.
 */
class Praesentation: public QObject
{
    Q_OBJECT
public:
    Praesentation();
    virtual ~Praesentation();
private:
    QMap<int, QString> slideReference;
    //! number of current slide (0 based)
    int currentSlide;
    //! number of total slide. 0 if no slide is present at all.
    int totalSlides;
    //! ID of presentation is generated from random.
    QString praesentationId;
    //! Base path of the presentation in filesystem.
    QString basePath;
    //! Represents if presentation is already running.
    bool running;
    //! Mutex for threadsafeness.
    QMutex accessLock;
public Q_SLOTS:
    //! Parse presentation from parameters of a recevied message.
    void parsePraesentation(QMap<QString, QVariant> params, QMap<QString, QString> types);
    //! Pack presentation to a message.
    Message* packPraesentation();
    //! Pack and add presentation to an existing message.
    Message* packPraesentation(Message * msg);
    //! Append slide to presentation.
    void appendSlide(QString path);
    //! Returns number of current slide (0 based).
    int getCurrentSlide();
    //! Returns total number of slides.
    int getTotalSlides();
    //! Sets presentation to slide x.
    void setSlide(int slide);
    //! Returns Presentation id.
    QString getPraesentationId();
    //! Returns base path.
    QString getBasepath();
    //! Resets all members.
    void reset();
    //! Stops presentation and emits signal.
    void stop();
private:
    void setBasepath();
signals:
    //! Signal with slide as image. Deprecated.
    void slideChanged(bb::cascades::Image);
    //! Signal with slide as url.
    void slideChangedUrl(QUrl url);
    //! Signal if presentation is parsed.
    void praesentationParsed(Message* response);
    //! Signal if parsing started/stopped.
    void parsing(bool active);
    //! Signal if presentation is ready to be started.
    void praesentationReady();
    //! Signal if presentation is running or not.
    void isRunning(bool active);


};

#endif /* PRAESENTATION_HPP_ */
