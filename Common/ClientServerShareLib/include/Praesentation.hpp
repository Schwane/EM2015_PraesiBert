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

#include "Message.hpp"

class Praesentation: public QObject
{
    Q_OBJECT
public:
    Praesentation();
    virtual ~Praesentation();
private:
    QMap<int, QString> slideReference;
    int currentSlide;
    int totalSlides;
    QString praesentationId;
    QString basePath;
    QMutex accessLock;
public Q_SLOTS:
    void parsePraesentation(QMap<QString, QVariant> params, QMap<QString, QString> types);
    //Message* packPraesentation();
    void appendSlide(QString path);
    int getCurrentSlide();
    int getTotalSlides();
    void setSlide(int slide);
    QString getPraesentationId();
private:
    void setBasepath();
signals:
    void slideChanged(bb::cascades::Image);
    void praesentationParsed(Message* response);
    void parsing(bool active);

};

#endif /* PRAESENTATION_HPP_ */
