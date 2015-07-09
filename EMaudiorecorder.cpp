/*
 * audiorecorder.cpp
 *
 *  Created on: 29.05.2015
 *      Author: arno
 */

#include "EMaudiorecorder.hpp"


namespace bb
{
    namespace EM2015
    {


EMaudiorecorder::EMaudiorecorder()
{
    initalized = false;
    armed = false;
    current_file = 0;
    record_running = false;

    recfile="file:///accounts/1000/shared/voice/recording.m4a";

    bb::system::InvokeDateTime ivdt();

    QTime global_time();


    err=0;
}



EMaudiorecorder::~EMaudiorecorder()
{
    bbrecorder.~AudioRecorder();
    rec_led.~Led();
    ivdt.~InvokeDateTime();
    global_time.~QTime();
}

char* EMaudiorecorder::record()
{


        bool err;

        int hour;
        int mins;
        int secs;
        int len;
        char address[] = "file:///accounts/1000/shared/voice/";
        char fformat[] = ".m4a";

        bbrecorder.reset();

        global_time = ivdt.time();
        global_time.start();

        hour = global_time.hour();
        mins = global_time.minute();
        secs = global_time.second();

        char timestamp_h [sizeof(hour)];
        char timestamp_m [sizeof(mins)];
        char timestamp_s [sizeof(secs)];
        itoa(hour, timestamp_h, 10);
        itoa(mins, timestamp_m, 10);
        itoa(secs, timestamp_s, 10);


        len = strlen(address) + strlen(timestamp_h) + strlen(timestamp_m) + strlen(timestamp_s) + strlen(fformat);

        char timestamp [len];

        strcpy(timestamp, address);
        strcat(timestamp, timestamp_h);
        strcat(timestamp, timestamp_m);
        strcat(timestamp, timestamp_s);
        strcat(timestamp, fformat);

        rec_led.setColor(bb::device::LedColor::Red);
        rec_led.flash();

        recfile.setUrl(timestamp);

        err=recfile.isValid();

        err = bbrecorder.setAudioChannelConfiguration(bb::multimedia::AudioChannelConfiguration::Mono);
        bbrecorder.setOutputUrl(recfile);
        err = bbrecorder.prepare();
        bbrecorder.record();

        return timestamp;


}

unsigned int EMaudiorecorder::stop()
{
    bbrecorder.pause();

    //bbrecorder.reset();

    rec_led.setColor(bb::device::LedColor::None);
    rec_led.cancel();

    initalized = false;

    return bbrecorder.duration();
}

void EMaudiorecorder::LED_TEST()
{
    rec_led.setColor(bb::device::LedColor::Green);

    rec_led.flash();

}



}
}
