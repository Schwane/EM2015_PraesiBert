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
        this->messageAuthenticator->setKey(messageAuthenticator->hmacSha1(this->symmetricKey, concatenatedNonce));

        this->authenticationTimer = new QTimer(this);

        QObject::connect(
                authenticationTimer, SIGNAL(timeout()),
                this, SLOT(authenticationTimeout())
                );

        authenticationTimer->start(5000); /* ms */
    }

    Master::~Master()
    {
        delete(authenticationTimer);
        delete(messageAuthenticator);
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

    Message* Master::handleSetSlide(QString commandName, Message* msg)
    {
        if(IS_COMMAND(commandName, CMD_SET_SLIDE))
        {
            if(msg->getParameters()->contains("slide"))
            {
                emit receivedSetSlide(msg->getParameters()->value("slide").toInt());
            }
        }

        return NULL;
    }

    void Master::authenticationTimeout()
    {
        WRITE_DEBUG("Master authentication timed out.")
        authenticationTimer->stop();
        emit authenticationFailed();
    }

    Message* Master::handleUnknownMessage(QString commandName, Message* msg)
    {
        Message * responseMessage = NULL;
        bool uIntConversionSuccessfull = FALSE;
        QString receiver = msg->getReceiver();
        unsigned int receiverId = 0;

        receiverId = msg->getReceiver().toUInt(&uIntConversionSuccessfull, 10);

        if("master" == msg->getSender() && uIntConversionSuccessfull)
        {
            emit forwardMessageToClient(msg, receiverId);
        }
        else
        {
            delete(msg);
        }
        return responseMessage;
    }

    Message* Master::handleAuthenticationAcknowledge(QString commandName, Message* msg)
    {
//        if(IS_COMMAND(commandName, CMD_ACK_RESPONSE))
//        {
//            if(ACCEPTED == authenticationStm(ReceivedAcknowledgeMessage))
//            {
//                delete(this->priorClientObject);
//            }
//        }
    }

    Message* Master::handleAuthenticationPhase3(QString commandName, Message* msg)
    {
        Message * responseMessage = NULL;

        if(IS_COMMAND(commandName, CMD_AUTH_PHASE3))
        {
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
                responseMessage = new Message(CMD_AUTH_PHASE4, "server", "master");
                responseMessage->addParameter("status", QString("ok"));
                responseMessage->addParameter("id", QString::number(this->getClientId()));
                this->authenticationTimer->stop();
                WRITE_DEBUG("Master authentication successfully finished.")
            }

        }

        delete(msg);

        return responseMessage;
    }

    Message* Master::handleStopPresentation(QString commandName, Message* msg)
    {
        Message * responseMessage = NULL;

        if(IS_COMMAND(commandName, CMD_STOP_PRAESENTATION))
        {
            emit stopPresentation();
        }

        return responseMessage;
    }

    Message* Master::handleReceivedAudio(QString commandName, Message* msg)
    {

        if(IS_COMMAND(DATA_AUDIO, commandName)
                && msg->getParameters()->contains("audio")
                )
        {
            QByteArray audioRecording = QByteArray::fromBase64(msg->getParameters()->value("audio").toByteArray());
            QString fileName;
            QString masterId;

            masterId.setNum(this->clientId);

            fileName.clear();
            fileName.append("master_id_");
            fileName.append(masterId);
            fileName.append("_at_");
            fileName.append(msg->getTimestamp().toString(Qt::ISODate));
            fileName.append(".m4a");

            emit writeAudioRecording(fileName, audioRecording);

            delete(msg);
        }

        return NULL;
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

    ClientType Master::getClientType()
    {
        return ClientType_Master;
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
