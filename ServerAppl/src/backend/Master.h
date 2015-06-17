/*
 * Master.h
 *
 *  Created on: 07.06.2015
 *      Author: sebastian
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <QObject>

#include <src/backend/Client.h>

namespace ServerAppl
{
    class Master : public Client
    {
        Q_OBJECT

    public:
        Master();
        virtual ~Master();

    signals:
        void receivedSlides();

    public slots:
        void onReceivedData(unsigned int receiverIdentifier);
        void onTransmitSlidesResponse(bool accepted);

    private:
        bool acceptSlides;
    };
} /* namespace ServerAppl */

#endif /* MASTER_H_ */
