/*
 * HDMI.hpp
 *
 *  Created on: 13.07.2015
 *      Author: arno
 */

#include "ExternalDisplay.hpp"
#include <QUrl>

#include <bb/cascades/Image>
#include <bb/ImageData>
#include <bb/utility/ImageConverter>
#include <img/img.h>


#ifndef HDMI_HPP_
#define HDMI_HPP_

/*
 * EXAMPLE:
 *    bb::EM2015::HDMI main_app(RES1024x768);
 *    QUrl path;
 *    path.setUrl("file:///accounts/1000/shared/camera/IMG1.jpg");
 *    main_app.show_slide(path);
 */

namespace bb
{
    namespace EM2015
    {

        class HDMI
        {
        public:
            HDMI(RESOLUTIONS_T hdmi_resolution);
            virtual ~HDMI();

            void show_slide(QUrl img_url);
            void show_slide(bb::ImageData img_data);
            void show_last_slide();

            /*
            void preload_slide(QUrl img_url);
            void next_slide(bb::ImageData* slide_img);
            */

        private:
            bool disp_open, preloaded_slide;
            ExternalDisplay extbert;
            QUrl _act_slide, _next_slide;
            bb::ImageData slide_img_preload, last_slight;

            RESOLUTIONS_T hdmires;


        };

    } /* namespace EM2015 */
} /* namespace bb */

#endif /* HDMI_HPP_ */
