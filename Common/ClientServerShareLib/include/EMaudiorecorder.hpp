/*
 * audiorecorder.hpp
 *
 *  Created on: 29.05.2015
 *      Author: arno
 */

#include <bb/multimedia/AudioRecorder.hpp>
#include <bb/device/Led>
#include <bb/device/LedColor>

#include <QUrl>
#include <QTime>
#include <bb/system/InvokeDateTime>
#include <bb/multimedia/AudioChannelConfiguration.hpp>
#include <string.h>


#include <QObject>

#include <stdlib.h>

namespace bb
{
    namespace EM2015
    {
        class EMaudiorecorder;
    }
}

#ifndef EMAUDIORECORDER_HPP_
#define EMAUDIORECORDER_HPP_

class bb::EM2015::EMaudiorecorder : public QObject
{
private:

    bool initalized;

    int err;

    QUrl recfile;
    QTime global_time;

    bb::multimedia::AudioRecorder bbrecorder;   /*Initalize recorder*/
    bb::device::Led rec_led;                    /*Initalize LED     */

    bb::system::InvokeDateTime ivdt;

public:
    EMaudiorecorder();
    ~EMaudiorecorder();

    char* record();
    unsigned int stop();

    void LED_TEST();

    bool armed;
    bool record_running;
    int current_file;
};


#endif /* EMAUDIORECORDER_HPP_ */
