/*
 * Presentation.h
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#ifndef PRESENTATION_H_
#define PRESENTATION_H_

#include <QHostAddress>
#include <bb/ImageData>
#include <QList>
#include <QObject>

#include <Message.hpp>

#include <src/backend/MessageHandlerInterface.h>

namespace ServerAppl
{
    /* Forward-Declaration */
    class Server;

    class PresentationController : public QObject, public MessageHandlerInterface
    {
        Q_OBJECT

    public:
        PresentationController(Server * server);
        virtual ~PresentationController();
        unsigned short countSlides();
        Message* handleReceivedMessage(QString commandName, Message* msg);

        /* message handlers */
        Message* handleSetSlide(QString commandName, Message* msg);

        signals:
            void showSlide(unsigned short number, QList<QHostAddress> *ipAddresses);
            void transmitSlidesResponse(bool accepted);

        public slots:
        /* slots */
            void onReceivedTransmitSlidesRequest(unsigned int transmitterIdentifier);
            void onReceivedSlides();
            void onReceivedStartPresentation();
            void onReceivedStopPresentation();

        public:
            /* data types */
            enum PresentationStmState
            {
                NotInitialized,
                NoSlidesPresent,
                WaitingForSlides,
                PresentationReady,
                PresentationRunning
            };

            enum PresentationStmEvent
            {
                Init,
                TransmitSlidesRequest,
                ReceivedSlides,
                StartPresentation,
                StopPresentation,
                ShowOtherSlide
            };

    private:
        bool presentationStm(PresentationStmEvent event);

        unsigned short currentSlide;
        Server * server;
    };

} /* namespace ServerAppl */

#endif /* PRESENTATION_H_ */
