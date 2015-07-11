/*
 * Master.cpp
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#include <commands.hpp>

#include <src/backend/Logger.h>
#include <src/backend/Master.h>

namespace ServerAppl
{
    Master::Master()
    {
        acceptSlides = FALSE;

    }

    Master::Master(UnspecifiedClient * priorClientObject, QString nonce1 )
    {
        QByteArray concatenatedNonce;
        this->currentAuthState = PHASE_1;
        this->priorClientObject = priorClientObject;
        this->server = priorClientObject->getServer();
        this->clientId = priorClientObject->getClientId();
        this->name = priorClientObject->getName();
        this->lastTimestamp = priorClientObject->getLastTimestamp();
        this->acceptSlides =  FALSE;
        this->nonce.part_1 = nonce1;
        this->nonce.part_2 = generateNonce(456);
        this->symmetricKey = "KATZE";
        this->messageAuthenticator = new MessageAuthenticator();
        concatenatedNonce.append(this->nonce.part_1);
        concatenatedNonce.append(this->nonce.part_2);
//        this->messageAuthenticator->hmacSha1(this->symmetricKey,concatenatedNonce);
        this->messageAuthenticator->setKey(symmetricKey);//this->symmetricKey);

        this->authenticationTimer = new QTimer(this);

        QObject::connect(
                authenticationTimer, SIGNAL(timeout()),
                this, SLOT(authenticationTimeout())
                );

        authenticationTimer->start(5000); /* ms */
    }

    Master::~Master()
    {
        this->authenticationTimer = new QTimer(this);
        // TODO Auto-generated destructor stub
    }

    Master* Master::createMaster(UnspecifiedClient * client, QString nonce1)
    {
        return new Master(client, nonce1);
    }

    QString Master::generateNonce(uint seed)
    {
        char buf[64];

        srand(seed);
        sprintf(buf, "%d", rand());

        return QString(buf);
    }

    Message* Master::handleDataPresentation(QString commandName, Message* msg)
    {
        Praesentation * presentation = new Praesentation();
        QMap<QString, QVariant> presentationParameterList = *(msg->getParameters());
        QMap<QString, QString> presentationParameterTypeList = *(msg->getParameterTypes());

        presentation->parsePraesentation(presentationParameterList, presentationParameterTypeList);

        emit receivedPresentation(presentation, presentationParameterList, presentationParameterTypeList);

        delete(msg);

        return NULL;
    }

    void Master::authenticationTimeout()
    {
        WRITE_DEBUG("Master authentication timed out.")
        authenticationTimer->stop();
        emit authenticationFailed();
    }

    Message* Master::handleReceivedMessage(QString commandName, Message* msg)
    {
        Message * responseMessage = NULL;

        return responseMessage;
    }

    Message* Master::handleAuthenticationAcknowledge(QString commandName, Message* msg)
    {
        Message * responseMessage;

        if(IS_COMMAND(commandName, CMD_ACK_RESPONSE))
        {
            if(ACCEPTED == authenticationStm(ReceivedAcknowledgeMessage))
            {
                delete(this->priorClientObject);
            }
        }

        delete(msg);

        return responseMessage;
    }

    // phase_3
    Message* Master::handleAuthenticationPhase3(QString commandName, Message* msg)
    {
        Message * responseMessage = NULL;

        if(IS_COMMAND(commandName, CMD_AUTH_PHASE3))
        {
            WRITE_DEBUG("handleAuthPhase 3")
            if(
                    (
                        !msg->getParameters()->contains(QString("status"))
                        ||(
                            msg->getParameters()->contains(QString("status"))
                            && "error" != msg->getParameterTypes()->value(QString("status"))
                        )
                    )
                    && PHASE_3 == authenticationStm(ReceivedPhase3Message)
                )
            {
                responseMessage = new Message(CMD_ACK_RESPONSE, "server", "master");
                responseMessage->addParameter(QString("status"), QString("ok"));

                WRITE_DEBUG("auth phase 3 successful")
            }

        }

        delete(msg);

        return responseMessage;
    }

    void Master::onReceivedData(unsigned int receiverIdentifier)
    {
        /* TODO examine received data-packet, create presentation-data-object
         * and emit signal with pointer to packet
         */

        if(receiverIdentifier == getClientId())
        {
            /* received slides */
            acceptSlides = FALSE;

            //TODO create presentation-data-object

            emit receivedSlides();
        }

    }

    NONCE Master::getNonce()
    {
        return this->nonce;
    }

    MessageAuthenticator* Master::getMessageAuthenticator()
    {
        return messageAuthenticator;
    }

    void Master::onTransmitSlidesResponse(bool accepted)
    {
        acceptSlides = accepted;
    }

    MasterAuthenticationState Master::authenticationStm(MasterAuthenticationEvent event)
    {
        MasterAuthenticationState newState = currentAuthState;

        switch(currentAuthState)
        {
            case PHASE_1:
                if(TransmittedPhase2Message == event)
                {
                    newState = PHASE_2;
                }
                break;
            case PHASE_2:
                if(ReceivedPhase3Message == event)
                {
                    newState = PHASE_3;
                }
                break;
            case PHASE_3:
                if(ReceivedAcknowledgeMessage == event)
                {
                    newState = ACCEPTED;
                }
                break;
            case ACCEPTED:

                break;
            case REJECTED:
                if(ReceivedPhase1Message == event)
                {
                    newState = PHASE_1;
                }
                break;
            default:
                if(ReceivedCorruptAuthenticationMessage == event)
                {
                    newState = REJECTED;
                }
                break;
        }

        currentAuthState = newState;

        return newState;
    }

} /* namespace ServerAppl */


