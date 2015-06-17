/*
 * Presentation.cpp
 *
 *  Created on: 31.05.2015
 *      Author: sebastian
 */

#include <QHostAddress>
#include <QList>

#include <src/backend/PresentationController.h>
#include <src/backend/Server.h>

namespace ServerAppl
{
    PresentationController::PresentationController(Server * server)
    {
        this->server = server;
        currentSlide = 0;
        presentationStm(Init);
    }

    PresentationController::~PresentationController()
    {

    }

    Message* PresentationController::handleReceivedMessage(QString commandName, Message* msg)
    {
        return NULL;
    }


    void PresentationController::onReceivedShowSlide(unsigned short number)
    {
        if(0 <= number
                && countSlides() > number
                && presentationStm(ShowOtherSlide)
        )
        {
            /* accepted */
//            QList<unsigned int> * identifiers = server->getAllClientIdentifiers();
//            emit transmitShowSlide(number, addresses);
        }
        else
        {
            /* rejected */
        }
    }

    void PresentationController::onReceivedStartPresentation()
    {
        if(presentationStm(StartPresentation))
        {
            /* accepted */
            //TODO transmit start-command to clients
        }
        else
        {
            /* rejected */
        }
    }

    void PresentationController::onReceivedStopPresentation()
    {
        if(presentationStm(StopPresentation))
        {
            //TODO transmit stop-command to clients
        }
    }

    void PresentationController::onReceivedTransmitSlidesRequest(unsigned int transmitterIdentifier)
    {
        if(server->getMasterClientIdentifier() == transmitterIdentifier
                && presentationStm(TransmitSlidesRequest))
        {
            /* accepted */
            //TODO transmit response to master
            emit transmitSlidesResponse(TRUE);
        }
        else
        {
            /* rejected */
            //TODO transmit notice of rejection to master
            emit transmitSlidesResponse(FALSE);
        }
    }

    /**
     * This is a dummy-method.
     *
     * In future this function shall be replaced by an own Presentation-class which holds data and
     * information about the presentation.
     */
    unsigned short PresentationController::countSlides()
    {
        return 3;
    }

    void PresentationController::onReceivedSlides()
    {
        //TODO safe received presentation-data-object! (will be given by event parameter)
        presentationStm(ReceivedSlides);
    }

    /**
     * This is the central STM for the presentation-process.
     */
    bool PresentationController::presentationStm(PresentationStmEvent event)
    {
        static PresentationStmState currentState = NotInitialized;
        bool retVal = FALSE;
        PresentationStmState newState;

        switch(currentState)
        {
            case NoSlidesPresent:
                if(TransmitSlidesRequest == event)
                {
                    // TODO check if allowed!
                    newState = WaitingForSlides;
                    retVal = TRUE;
                }
                break;
            case WaitingForSlides:
                if(ReceivedSlides)
                {
                    newState = PresentationReady;
                    retVal = TRUE;
                }
                break;
            case PresentationReady:
                if(StartPresentation == event)
                {
                    // TODO only if slides transmitted to the clients?
                    newState = PresentationRunning;
                    retVal = TRUE;
                }
                else if(TransmitSlidesRequest == event)
                {
                    // TODO abort transmitting slides to clients if necessary

                    newState = WaitingForSlides;
                    retVal = TRUE;
                }
                break;
            case PresentationRunning:
                if(ShowOtherSlide == event)
                {
                    newState = PresentationRunning;
                    retVal = TRUE;
                }
                break;
            case NotInitialized:
                if(Init == event)
                {
                    newState = NoSlidesPresent;
                    retVal = TRUE;
                }
                break;
            default:

                break;
        }

        switch(newState)
        {
            case NoSlidesPresent:
            case NotInitialized:
            case WaitingForSlides:
                currentState = newState;
                break;
            default:

                break;
        }
        return retVal;
    }
} /* namespace ServerAppl */
