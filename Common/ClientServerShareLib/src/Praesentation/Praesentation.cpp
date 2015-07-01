/*
 * Praesentation.cpp
 *
 *  Created on: 11.06.2015
 *      Author: JCB
 */

#include "include/Praesentation.hpp"

Praesentation::Praesentation()
{
    currentSlide = -1;
    totalSlides = 0;
    praesentationId = QDate::currentDate().toString("dd_MM_yyyy_");
    QString randTag;
    qsrand(QDateTime::currentMSecsSinceEpoch());
    randTag.sprintf("%08X", qrand()*RAND_MAX + qrand());
    praesentationId.append(randTag);
    setBasepath();

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

    slideReference.insert(totalSlides, path);
    totalSlides += 1;
}

void
Praesentation::setBasepath()
{
    basePath.clear();
    basePath = QDir::homePath();
    basePath.append("/");
    basePath.append(praesentationId);
    basePath.append("/");
}

void
Praesentation::parsePraesentation(QMap<QString, QVariant> params, QMap<QString, QString> types)
{

    if (params.contains("praesentationID") && types.contains("praesentationID") && types.value("praesentationID") == "string")
    {
        praesentationId = params.value("praesentationID").toString();
        setBasepath();
    }
    else
    {
        //@TODO: Ausnahme wenn die Nachricht nicht gültig war
    }

    if (params.contains("totalSlides") && types.contains("totalSlides") && types.value("totalSlides") == "integer")
    {
        totalSlides = params.value("totalSlides").toInt();
    }
    else
    {
        //@TODO: Ausnahme wenn die Nachricht nicht gültig war
    }
    QString slideID;
    QByteArray image;
    for (int i = 0; i < totalSlides; i++)
    {
        slideID.clear();
        slideID.sprintf("%03u", i); /*Check if the slide is contained in the message*/
        if (params.contains(slideID) && types.contains(slideID) && types.value(slideID) == "b64")
        {
            QVariant imgVar = params.value(slideID);
            QByteArray imgBytes;
            //convert byte array from b64 to image
            imgVar.convert(QVariant::String);
            imgBytes.append(imgVar.toString());
            imgBytes = QByteArray::fromBase64(imgBytes);

            QImage img;

            img = QImage::fromData(imgBytes, "JPG");
            QString path;
            //save slide and add to the reference
            path = basePath;
            path.append(slideID);
            path.append(".jpg");
            img.save(path,"JPG",10);

            slideReference.insert(i, path);
        }
        else
        {
            //@TODO: Ausnahme wenn die Nachricht nicht gültig war
        }
    }
}

void
Praesentation::setSlide(int slide)
{
    if (slide > totalSlides)
        return;
    if (slide < 0)
        return;
    currentSlide = slide;

    QString path = slideReference.value(slide);

    QImage img(path); /* Load image and convert it to cascades image for the gui */
    QImage swappedImage = img.rgbSwapped();
    if(swappedImage.format() != QImage::Format_RGB32) {
        swappedImage = swappedImage.convertToFormat(QImage::Format_RGB32);
    }
    const bb::ImageData imgData= bb::ImageData::fromPixels(swappedImage.bits(), bb::PixelFormat::RGBX, swappedImage.width(), swappedImage.height(), swappedImage.bytesPerLine());
    emit slideChanged(bb::cascades::Image(imgData));
}
