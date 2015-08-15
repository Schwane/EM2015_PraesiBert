/*
 * Praesentation.cpp
 *
 *  Created on: 11.06.2015
 *      Author: JCB
 */

#include "include/Praesentation.hpp"

Praesentation::Praesentation()
{
    reset();
}

Praesentation::~Praesentation()
{
    // TODO Auto-generated destructor stub
}

QString
Praesentation::getPraesentationId()
{
    return praesentationId;
}

void
Praesentation::reset()
{
    slideReference.clear();
    currentSlide = -1;
    totalSlides = 0;
    running = false;
    praesentationId = QDate::currentDate().toString("dd_MM_yyyy_"); //ID contains random number and current date
    QString randTag;
    qsrand(QDateTime::currentMSecsSinceEpoch());
    randTag.sprintf("%08X", qrand()*RAND_MAX + qrand());
    praesentationId.append(randTag);
    setBasepath();
}


void
Praesentation::stop()
{
    running = false;
    emit isRunning(false); //signal the stop
}



int
Praesentation::getCurrentSlide()
{
    return currentSlide;
}

int
Praesentation::getTotalSlides()
{
    return totalSlides;
}

void
Praesentation::appendSlide(QString path)
{
    slideReference.insert(totalSlides, path); //save url and increase total slides
    totalSlides += 1;
}

void
Praesentation::setBasepath()
{
    basePath.clear();
    basePath = QDir::currentPath(); //app folder
    basePath.append("/shared/misc/presentations/"); //we want to add it in shared so user can access it later
    basePath.append(praesentationId); //new folder for each presi
    basePath.append("/");
    QDir dir(basePath);
    dir.mkpath(dir.absolutePath()); //create all necessary folders
    QDir voiceDir(basePath + "voice/"); //add folder for voice recordings
    voiceDir.mkpath(voiceDir.absolutePath());
}

void
Praesentation::parsePraesentation(QMap<QString, QVariant> params, QMap<QString, QString> types)
{
    reset(); //reset the presentation
    //check if id is correct and set it, create new base path based on new id.
    if (params.contains("praesentationID") && types.contains("praesentationID") && types.value("praesentationID") == "string")
    {
        praesentationId = params.value("praesentationID").toString();
        setBasepath();
    }
    else
    {
        //@TODO: Ausnahme wenn die Nachricht nicht gültig war
    }
    //check the total number of slides
    if (params.contains("totalSlides") && types.contains("totalSlides") && types.value("totalSlides") == "integer")
    {
        totalSlides = params.value("totalSlides").toInt(); // ... and set it
    }
    else
    {
        //@TODO: Ausnahme wenn die Nachricht nicht gültig war
    }
    QString slideID;
    QByteArray image;
    emit parsing(true); //signal that parsing starts
    //For every slide in ascending order
    for (int i = 0; i < totalSlides; i++)
    {
        slideID.clear();
        slideID.sprintf("%03u", i); /*Check if the slide is contained in the message*/
        if (params.contains(slideID) && types.contains(slideID) && types.value(slideID) == "b64")
        {
            QVariant imgVar = params.value(slideID);
            QByteArray imgBytes;
            //convert byte array from b64 to image
            imgBytes = QByteArray::fromBase64(imgVar.toByteArray());



            QString path;
            //save slide and add to the reference
            path = basePath;
            path.append(slideID);
            path.append(".jpg");
            QFile f(path);

            //try to open
            if(!f.open(QIODevice::WriteOnly))
            {
                qDebug() << "Error opening File: " << path;
                break;
            }
            //write slide to file
            f.write(imgBytes);
            slideReference.insert(i, path);
        }
        else
        {
            //@TODO: Ausnahme wenn die Nachricht nicht gültig war
        }
    }
    emit parsing(false);
    emit praesentationReady(); //signal that parsing is done and presentation is ready
}

void
Praesentation::setSlide(int slide)
{
    //check boundaries
    if (slide > totalSlides)
        return;
    if (slide < 0)
        return;
    accessLock.lock();
    currentSlide = slide;

    QString path = slideReference.value(slide); //get path of slide

    //We decided to only emit the url not the whole image due to performance issues and compatibility of HDMI device implementation

    //const bb::ImageData imgData = bb::utility::ImageConverter::decode(QUrl(path));

    accessLock.unlock();
    //If presnetation was not running it is now!
    if (!running && slide >= 0)
    {
        running = true;
        emit isRunning(true);
    }
    //emit slideChanged(bb::cascades::Image(imgData));
    emit slideChangedUrl(QUrl(path)); // emit url
}

Message*
Praesentation::packPraesentation(Message * msg)
{
    //Add ID and total slides
    msg->addParameter("praesentationID", praesentationId);
    msg->addParameter("totalSlides", totalSlides);

    QString slideID;
    //For every slide, ...
    for (int i = 0; i < totalSlides; i++)
    {
        slideID.clear();
        slideID.sprintf("%03u", i); /*Check if the slide is contained in the message*/

        QByteArray imgBytes;
        QFile F(slideReference.value(i)); //read the slide's url
        F.open(QIODevice::ReadOnly); //read the slide from dile
        imgBytes = F.readAll();

        msg->addParameter(slideID, imgBytes); //append it to the message
    }
    return msg;
}

Message*
Praesentation::packPraesentation()
{
    Message *msg = new Message(DATA_PRAESENTATION, "master", "server");

    return packPraesentation(msg);
}

QString
Praesentation::getBasepath()
{
    return basePath;
}
