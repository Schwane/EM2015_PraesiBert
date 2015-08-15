/*
 * audiorecorder.cpp
 *
 *  Created on: 29.05.2015
 *      Author: arno
 */

#include "include/EMaudiorecorder.hpp"


namespace bb
{
    namespace EM2015
    {


        /*!
         * @brief Initalizes the EMaudiorecorder v 1.0.
         *
         * @details Initalizes the EMaudiorecorder: global variables and classes.
         *
         *
         */

        EMaudiorecorder::EMaudiorecorder()
        {
            initalized = false;
            armed = false;
            current_file = 0;
            record_running = false;

            recfile="file:///accounts/1000/shared/voice/recording.m4a"; // Set default file location

            bb::system::InvokeDateTime ivdt();                          // Access system time

            QTime global_time();                                        // Time Object

            /*
             *   ATM for debugging purpose only: error flag
             */
            err=0;
        }

        /*!
         * @brief destructor
         */

        EMaudiorecorder::~EMaudiorecorder()
        {
            bbrecorder.~AudioRecorder();
            rec_led.~Led();
            ivdt.~InvokeDateTime();
            global_time.~QTime();
        }

        /*!
         * @brief Start the recording.
         *
         * @details This method implements the whole process of setting the file name, turning on the LED, preparing the recorder and starting it.
         *
         * @return Adress of recording location as char array.
         *
         */

        char* EMaudiorecorder::record()
        {

            /*
             * Declaration of local variables
             */

            bool err;                                               // Error flag, debug only

            int hour, mins, secs, len;                              // System time and str_length

            char address[] = "file:///accounts/1000/shared/voice/"; // Recording location
            char fformat[] = ".m4a";                                // File format

            /*
             * First we have to reset the AudioRecorder
             */

            bbrecorder.reset();

            /*
             * Calculate the filename from the system time
             */

            global_time = ivdt.time();
            global_time.start();

            hour = global_time.hour();
            mins = global_time.minute();
            secs = global_time.second();

            /*
             * We have to allocate the timestamp_* variables here,
             * for we don't know the size before!
             */

            char timestamp_h [sizeof(hour)];
            char timestamp_m [sizeof(mins)];
            char timestamp_s [sizeof(secs)];

            /*
             * Type conversion
             */

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

            /*
             * Set the LED to 'record mode'
             */

            rec_led.setColor(bb::device::LedColor::Red);
            rec_led.flash();

            /*
             * Set the recording location and check for errors
             */

            recfile.setUrl(timestamp);
            err=recfile.isValid();

            err = bbrecorder.setAudioChannelConfiguration(bb::multimedia::AudioChannelConfiguration::Mono);
            bbrecorder.setOutputUrl(recfile);

            /*
             * Transfer URL to AudioRecorder and begin with the recording.
             */

            err = bbrecorder.prepare();         // The URL is transferred here
            bbrecorder.record();                // Actual recording process

            return timestamp;


        }

        /*!
         * @brief Stop the recording.
         *
         * @details This method stops the recording and toggles the LED off.
         *
         * @return Returns the duration of the recording for audio editing purpose.
         *
         */
        unsigned int EMaudiorecorder::stop()
        {
            bbrecorder.pause();

            //bbrecorder.reset();

            rec_led.setColor(bb::device::LedColor::None);
            rec_led.cancel();

            initalized = false;

            return bbrecorder.duration();
        }

        /*!
         * @brief LED test.
         *
         * @details This method toggles the green LED on. Unused in program, for testing purpose.
         *
         */

        void EMaudiorecorder::LED_TEST()
        {
            rec_led.setColor(bb::device::LedColor::Green);
            rec_led.flash();
        }



    }
}
