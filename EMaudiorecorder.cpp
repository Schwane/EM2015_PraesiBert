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
    err=0;
}



EMaudiorecorder::~EMaudiorecorder()
{
    bbrecorder.~AudioRecorder();
    rec_led.~Led();
}



void EMaudiorecorder::initalize()
{
   bbrecorder.reset();
  err = bbrecorder.setAudioChannelConfiguration(bb::multimedia::AudioChannelConfiguration::Mono);
  bbrecorder.setOutputUrl(recfile);
  err = bbrecorder.prepare();

  if(err==0)
    initalized=true;


}



void EMaudiorecorder::record()
{
    if(initalized)
    {
        rec_led.setColor(bb::device::LedColor::Red);
        rec_led.flash();

        bbrecorder.record();

    }


}

unsigned int EMaudiorecorder::stop()
{
    bbrecorder.reset();

    rec_led.setColor(bb::device::LedColor::None);
    rec_led.cancel();

    initalized = false;

    return bbrecorder.duration();
    return 0;
}

void EMaudiorecorder::LED_TEST()
{
    rec_led.setColor(bb::device::LedColor::Green);

    rec_led.flash();


}


}
}
