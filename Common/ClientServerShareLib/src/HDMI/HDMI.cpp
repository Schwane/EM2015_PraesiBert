/*
 * HDMI.cpp
 *
 *  Created on: 13.07.2015
 *      Author: arno
 */

#include "include/HDMI.hpp"

namespace bb
{
    namespace EM2015
    {

        /*!
         * @brief Initalizes the HDMI
         *
         * @details Initalizes the HDMI: global variables and classes.
         *
         * @arg Screen resolution (RESOLUTIONS_T)
         *
         */


        HDMI::HDMI(RESOLUTIONS_T hdmi_resolution)
        {
           ExternalDisplay extbert();
           _act_slide = NULL;
           _next_slide = NULL;
           last_slight = bb::utility::ImageConverter::decode(QUrl("assets:///img/before_start.png"));
           preloaded_slide = false;
           disp_open = false;
           hdmires = hdmi_resolution;
        }

        HDMI::~HDMI()
        {
            // TODO Auto-generated destructor stub
        }

        /*!
         * @brief Show slide
         *
         * @details Update the output of HDMI, link to the picture in argument.
         *
         * @arg QUrl img_url
         * @return -none-
         *
         */

        void HDMI::show_slide(QUrl img_url)
        {
            if(disp_open == false)
            {
                extbert.setResolution(hdmires);
                extbert.open();
                disp_open = true;
            }

            bb::ImageData slide_img = bb::utility::ImageConverter::decode(QUrl(img_url));
            extbert.showImage(slide_img);

        }

    /*    void HDMI::preload_slide(QUrl img_url)
        {
            slide_img_preload = bb::utility::ImageConverter::decode(QUrl(img_url));
            preloaded_slide = true;
        }

        void HDMI::next_slide(bb::ImageData* slide_img)
        {
            if(preloaded_slide)
            {
                slide_img = &slide_img_preload;
            }
        }

        */

        /*!
         * @brief Show last slide
         *
         * @details Show the last slide ("Presentation finished")
         *
         * @return -none-
         *
         */

        void HDMI::show_last_slide()
        {
            if(disp_open == false)
               {
                   extbert.setResolution(hdmires);
                   extbert.open();
                   disp_open = true;
               }
            extbert.showImage(last_slight);
        }



    } /* namespace EM2015 */
} /* namespace bb */
