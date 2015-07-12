/*
 * CameraProcessor.cpp
 *
 *  Created on: 15.06.2015
 *      Author: Inga
 */

#include "CameraProcessor.hpp"

//Initialize the static control variable m_busy
bool CameraProcessor::m_busy = false;

//Initialize the static counter for not processed frames
unsigned int CameraProcessor::m_not_processed_counter = 0;

//Constructor
/* To optimize the performance, try changing the following parameters:
 * m_framerate: With a framerate of 16 (or less) frames per second every frame gets reliably processed,
 *              but maybe it could be set higher for better results.
 * m_threshold: The threshold determines when a change in pixel value is deemed significant.
 *              The pixel values range from 255 (white) to 0 (black). Too small changes from one
 *              frame to the next may be caused by shadows etc., so they should be ignored.
 * m_interval_percentage: Determines the percentage of a frame that counts as left or right (where
 *                        gestures start and finish) and the interval in which a gesture is regarded
 *                        continued from one frame to the next.
 */
CameraProcessor::CameraProcessor(QObject* parent) :
        QObject(parent),
        m_handle(CAMERA_HANDLE_INVALID),
        m_frametype(CAMERA_FRAMETYPE_NV12),
        m_framerate(20.0),
        m_rotation(90),
        m_threshold(20),
        m_reset_counter(0),
        m_previous_position(0),
        m_status(NOTHING),
        m_interval_percentage(0.05)
{
    //Connect the imageReady signal, emitted during the viewfinder_callback(), to the process() function
    //ConnectionType is QueuedConnection, which means the slot will be executed in the receiver's thread
    QObject::connect(this, SIGNAL(imageReady()), this, SLOT(process()), Qt::QueuedConnection);
    qDebug() << "CameraProcessor: signal imageReady() connected to slot process()";
    qDebug() << "CameraProcessor object created";
}

//Destructor
CameraProcessor::~CameraProcessor()
{
    //If the camera has been opened, stop the viewfinder and close the camera
    if(m_handle != CAMERA_HANDLE_INVALID) {
        stop();
    }
}

//Slot: starts the camera and data processing
void CameraProcessor::start()
{
    qDebug() << "Slot CameraProcessor::start() called";
    openCamera();
    startVf();
}

//Slot: stops the camera and data processing
void CameraProcessor::stop()
{
    qDebug() << "Slot CameraProcessor::stop() called";
    qDebug() << "Number of frames that were not processed: " << m_not_processed_counter;
    stopVf();
    closeCamera();
}

void CameraProcessor::openCamera()
{
    qDebug() << "CameraProcessor::openCamera() called";
    camera_error_t err;
    //Open front camera with read & write access
    err = camera_open(CAMERA_UNIT_FRONT, CAMERA_MODE_RW | CAMERA_MODE_ROLL, &m_handle);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not open front camera unit");
    }
}

void CameraProcessor::closeCamera()
{
    qDebug() << "CameraProcessor::closeCamera() called";
    camera_close(m_handle);
    m_handle = CAMERA_HANDLE_INVALID;
}

void CameraProcessor::startVf()
{
    qDebug() << "CameraProcessor::startVf() called";
    camera_error_t err;
    //Set the viewfinder mode (maybe CAMERA_VFMODE_CONTINUOUS_BURST is better)
    err = camera_set_vf_mode(m_handle, CAMERA_VFMODE_PHOTO);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not set viewfinder mode");
        return;
    }
    qDebug() << "CameraProcessor::startVf(): viewfinder mode set to CAMERA_VFMODE_PHOTO";

    //Check if CAMERA_FRAMETYPE_NV12 is supported
    //setFrametype();

    //Get the lowest supported resolution
    setLowestResolution();
    /* It would have been nice to use the lowest supported resolution, but for some unknown
     * reason, on the Blackberry Z10 the viewfinder only starts with 3 of the 12 supported
     * resolutions, although all 12 resolutions can be set.
     * The lowest working resolution is 288 x 512
     */
    m_resolution.width = 288;
    m_resolution.height = 512;
    qDebug() << "CameraProcessor::startVf(): m_resolution set to " << m_resolution.width << " x "
            << m_resolution.height;
    //Set the framerate
    setFramerate();
    qDebug() << "CameraProcessor::startVf(): m_framerate set to " << m_framerate;
    //Set the rotation
    /* The Blackberry Z10 only supports a rotation of 90 degrees, so for this device this
     * function call is quite useless, but in general this code although supports any other
     * square(!) rotation
     */
    setRotation();
    qDebug() << "CameraProcessor::startVf(): m_rotation set to " << m_rotation;

    //Set properties for the viewfinder (separately for more precise errors)

    //frametype = CAMERA_FRAMETYPE_NV12
    err = camera_set_vf_property(m_handle, CAMERA_IMGPROP_FORMAT, m_frametype);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not set viewfinder property: frametype");
        return;
    }
    //resolution
    err = camera_set_vf_property(m_handle,
            CAMERA_IMGPROP_WIDTH, m_resolution.width,
            CAMERA_IMGPROP_HEIGHT, m_resolution.height);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not set viewfinder property: resolution");
        return;
    }
    //framerate
    err = camera_set_vf_property(m_handle, CAMERA_IMGPROP_FRAMERATE, m_framerate);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not set viewfinder property: framerate");
        return;
    }
    //rotation
    err = camera_set_vf_property(m_handle, CAMERA_IMGPROP_ROTATION, m_rotation);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not set viewfinder property: rotation");
        return;
    }

    //no window creation
    err = camera_set_vf_property(m_handle, CAMERA_IMGPROP_CREATEWINDOW, 0);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not set viewfinder property: create window");
        return;
    }

    qDebug() << "CameraProcessor::startVf(): viewfinder properties set";

    //Initialize the openCV matrices needed for the viewfinder_callback()-function
    initMatrices();

    //Start the viewfinder with the callback function viewfinder_callback() as parameter
    err = camera_start_viewfinder(m_handle, &viewfinder_callback, NULL, (void*) this);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not start viewfinder");
        return;
    }
    qDebug() << "CameraProcessor::startVf(): viewfinder started";
}

//Function camera_get_supported_vf_frametypes is missing in the camera_api-library!
/*void CameraProcessor::setFrametype()
{
    camera_error_t err;

    //First call the function to get the supported frametypes
    //in presizing mode to get the needed array size
    unsigned int num;
    err = camera_get_supported_vf_frametypes(m_handle, 0, &num, NULL);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder frametypes");
        return;
    }
    camera_frametype_t frametypes[num];

    //Then call it again
    err = camera_get_supported_vf_frametypes(m_handle, num, &num, frametypes);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder frametypes");
        return;
    }

    //Check if nv12 is supported
    bool supported = false;
    for (unsigned int i = 1; i < num; i++) {
        if (frametypes[i] == m_frametype) {
            supported = true;
        }
    }

    if(!supported) {
        qDebug() << "CAMERA_FRAMETYPE_nv12 is NOT supported";
    }
}*/

void CameraProcessor::setLowestResolution()
{
    camera_error_t err;

    //First call the function to get the supported viewfinder resolutions
    //in presizing mode to get the needed array size
    unsigned int num;
    err = camera_get_supported_vf_resolutions(m_handle, 0, &num, NULL);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder resolutions");
        return;
    }
    camera_res_t resolutions[num];

    //Then call it again
    err = camera_get_supported_vf_resolutions(m_handle, num, &num, resolutions);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder resolutions");
        return;
    }

    //Search the lowest resolution
    camera_res_t lowest = resolutions[0];
    for (unsigned int i = 1; i < num; i++) {
        if (resolutions[i].width <= lowest.width && resolutions[i].height <= lowest.height) {
            lowest = resolutions[i];
        }
    }

    //Set the member variable to this resolution
    m_resolution = lowest;
}

void CameraProcessor::setFramerate()
{
    camera_error_t err;

    //First call the function to get the supported viewfinder framerates
    //in presizing mode to get the needed array size
    unsigned int num;
    bool maxmin;
    err = camera_get_supported_vf_framerates(m_handle, CAMERA_FRAMETYPE_UNSPECIFIED, 0, &num, NULL,
            &maxmin);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder framerates");
        return;
    }
    double framerates[num];

    //Then call it again
    err = camera_get_supported_vf_framerates(m_handle, CAMERA_FRAMETYPE_UNSPECIFIED, num, &num,
            framerates, &maxmin);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder framerates");
        return;
    }

    //If the maxmin flag is set, the returned framerates are the maximum and the minimum framerate
    if (maxmin) {
        //If m_framerate is greater than the maximum supported framerate, set it to maximum
        if (m_framerate > framerates[0])
            m_framerate = framerates[0];
        //If smaller, set it to minimum
        else if (m_framerate < framerates[1])
            m_framerate = framerates[1];
        //Else keep m_framerate as it is
    }
    //Else only discrete framerates (contained in the array) are supported
    else {
        double minDistance;
        int indexMinDistance;
        bool supported = false;

        for (unsigned int i = 0; i < num; i++) {
            //If m_framerate is a supported value, keep it and break the loop
            if (m_framerate == framerates[i]) {
                supported = true;
                break;
            }
            //Else search for the closest supported framerate
            else {
                double temp = abs(framerates[i] - m_framerate);
                if (temp < minDistance) {
                    minDistance = temp;
                    indexMinDistance = i;
                }
            }
        }

        //If m_framerate was not found, set it to the closest supported framerate
        if (!supported)
            m_framerate = framerates[indexMinDistance];
    }
}

void CameraProcessor::setRotation() {
    //Preferred rotation: m_rotation
    camera_error_t err;
    //First call the function to get the supported viewfinder rotations
    //in presizing mode to get the needed array size
    unsigned int num;
    bool nonsquare;
    err = camera_get_supported_vf_rotations(m_handle, CAMERA_FRAMETYPE_UNSPECIFIED, 0, &num, NULL, &nonsquare);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder rotations");
        return;
    }
    unsigned int rotations[num];
    //Then call it again
    err = camera_get_supported_vf_rotations(m_handle, CAMERA_FRAMETYPE_UNSPECIFIED, num, &num, rotations, &nonsquare);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not get supported viewfinder resolutions");
        return;
    }

    //Check if the preferred rotation is supported, if not, set it to the default value stored in rotations[0]
    bool supported = false;
    for (unsigned int i = 0; i < num; i++) {
        if (m_rotation == rotations[i]) {
            supported = true;
            break;
        }
    }
    if(!supported) {
        m_rotation = rotations[0];
    }
}

//Initializes the matrices m_previous_frame, m_actual_frame and m_difference
//with the correct size
void CameraProcessor::initMatrices()
{
    //Since the matrices will only contain the middle row of a frame, number of rows is 1
    unsigned int rows = 1;
    unsigned int cols = m_resolution.width;

    //Initialize the matrices with 255 (= white) for each element for the frames and 0 for the difference
    m_actual_frame = cv::Mat(rows, cols, CV_8UC1, cv::Scalar(255));
    m_previous_frame = cv::Mat(rows, cols, CV_8UC1, cv::Scalar(255));
    m_difference = cv::Mat(rows, cols, CV_8UC1, cv::Scalar(0));
}

void CameraProcessor::stopVf()
{
    qDebug() << "CameraProcessor::stopVf() called";
    camera_stop_viewfinder(m_handle);

}

void CameraProcessor::viewfinder_callback(camera_handle_t handle, camera_buffer_t* buffer,
        void* arg)
{
    //qDebug() << "viewfinder_callback started";
    //To avoid memory conflicts only proceed if the control variable m_busy is not set
    if(!m_busy) {
        //Set a pointer to the CameraProcessor instance
        CameraProcessor* inst = (CameraProcessor*) arg;
        //Read the buffer into an opencv matrix (deep copy)
        inst->m_image = cv::Mat(buffer->framedesc.nv12.height, buffer->framedesc.nv12.width, CV_8UC1, (void *) buffer->framebuf, buffer->framedesc.nv12.stride).clone();
        //Emit a signal that a new image is ready for processing
        inst->emit imageReady();
    }
    //Else increase the not-processed-counter
    else  {
        m_not_processed_counter++;
    }
    //qDebug() << "viewfinder_callback finished";
}

void CameraProcessor::process()
{
    //Set control variable to prevent another function call while a previous frame is still processed
    m_busy = true;
    //qDebug() << "CameraProcessor:process() started";

    //Check if height and width of the image are equal to m_resolution
    if(m_rotation == 0 || m_rotation == 180) {
        if (m_image.rows != (int) m_resolution.height || m_image.cols != (int) m_resolution.width) {
            emit error("Error: height or width of the buffered image don't match m_resolution");
            return;
        }
    }
    if(m_rotation == 90 || m_rotation == 270) {
        if (m_image.rows != (int) m_resolution.width || m_image.cols != (int) m_resolution.height) {
            emit error("Error: height or width of the buffered image don't match m_resolution");
            return;
        }
    }

    //If necessary rotate the image
    cv::Mat image_r;
    if(m_rotation == 90) {
        cv::flip(m_image, image_r, 0); //flipcode 0 = flip horizontally
        cv::transpose(image_r, m_image);
    }
    if(m_rotation == 180) {
        cv::flip(m_image, image_r, -1); //flipcode -1 = flip horizontally and vertically
        m_image = image_r;
    }
    if(m_rotation == 270) {
        cv::flip(m_image, image_r, 1); //flipcode 1 = flip vertically
        cv::transpose(image_r, m_image);
    }
    image_r.release();
    //qDebug() << "CameraProcessor:process(): image rotation done";

    //Reduce the image to the middle row (this references still the same data)
    m_actual_frame = m_image.row((int) (m_image.rows / 2) );

    //Compare the image to the previous frame
    m_difference = m_previous_frame - m_actual_frame;

    /* Regarding the values:
     * The frames have pixel values between 0 and 255 - stored in unsigned chars - where 255 is white
     * and 0 is black.
     * If m_difference is > 0, the previous frame was lighter at that pixel than the actual frame
     * If m_difference equals 0, the previous frame was either darker or the same at that pixel
     */

    //qDebug() << "m_difference =" << m_difference.at<uchar>(0) << m_difference.at<uchar>(36) << m_difference.at<uchar>(72) << m_difference.at<uchar>(108) << m_difference.at<uchar>(144) << m_difference.at<uchar>(180) << m_difference.at<uchar>(216) << m_difference.at<uchar>(252) << m_difference.at<uchar>(287);

    //Save the image as the new previous frame (deep copy)
    m_previous_frame = m_actual_frame.clone();

    //qDebug() << "CameraProcessor:process(): all matrices updated";

    //Check each pixel in the difference matrix
    bool changed = false;
    for(int i = 0; i < m_difference.cols; i++) {
        //If the pixel's value exceeds the threshold, it is significantly darker than before - set it to 1
        if(m_difference.at<uchar>(i) >= m_threshold) {
            m_difference.at<uchar>(i) = 1;
            if(!changed)
                changed = true;
        }
        //Else set it to 0
        else {
            m_difference.at<uchar>(i) = 0;
        }
    }
    //qDebug() << "CameraProcess:process(): evaluation of difference done, changed =" << changed;

    //If there was no change detected, check the status, maybe change it, and return
    if(!changed) {
        //If the status is LEFT or RIGHT, it should only be kept for a few frames without changes
        if(m_status != NOTHING) {
            int max = (int) m_framerate / 8;
            if(m_reset_counter >= max) {
                m_status = NOTHING;
                m_reset_counter = 0;
            }
            else {
                m_reset_counter++;
            }
        }
        //Set control variable and return
        m_busy = false;
        return;
    }

    //Else analyse the changes

    //The interval serves the following purposes:
    // - One interval from the start of the row counts as "left"
    // - One interval from the end of the row counts as "right"
    // - One interval from a previous position of an object counts as "continuous move"
    int interval = (int) (m_difference.cols * m_interval_percentage);

    //Depending on the status (NOTHING, LEFT or RIGHT)
    switch (m_status) {

       //If there were no prior indications of any gesture
       case NOTHING:
       {
           bool left = false;
           bool right = false;
           //For each pixel
           for(int i = 0; i < m_difference.cols; i++) {
               //If it's 1, there was an object/movement detected
               if(m_difference.at<uchar>(i) == 1) {
                   //If the 1 is in the left part, set left = true
                   if(i < interval)
                       left = true;
                   //If it's in the right part, set right = true
                   if(i > (m_difference.cols - 1 - interval) )
                       right = true;
               }
           }

           //If both left and right are true, something is odd and the status should not change
/*           if(left && right) {
               qDebug() << "left & right both true!";
               m_busy = false;
               return;
           }*/

           //Else set status to LEFT or RIGHT
           if(left) {
               m_status = RIGHT;
           }
           if(right) {
               m_status = LEFT;
           }

           //If there are indications of a gesture, save the index of the leftmost or rightmost 1
           switch(m_status) {
               case RIGHT:
                   for(int i = 0; i < m_difference.cols - 1; i++) {
                       if(m_difference.at<uchar>(i) == 1 && m_difference.at<uchar>(i+1) == 0)
                           m_previous_position = i;
                   }
                   break;
               case LEFT:
                   for(int i = m_difference.cols - 1; i >= 1; i--) {
                       if(m_difference.at<uchar>(i) == 1 && m_difference.at<uchar>(i-1) == 0)
                           m_previous_position = i;
                   }
                   break;
               case NOTHING:
                   break;
           }
       }
       break;

       //If an assumed gesture to the right is in process
       case RIGHT:
       {
           bool continued = false;
           //Start at the previous position of the rightmost 1 and check if there is a new 1 to the right
           for(int i = m_previous_position; i <= (m_previous_position + interval) && i < m_difference.cols; i++) {
               if(m_difference.at<uchar>(i) == 1)
                   continued = true;
           }
           //If no 1 was found, change status to NOTHING
           if(!continued) {
               m_status = NOTHING;
               m_previous_position = 0;
           }
           //Else keep the status and check if the gesture is finished
           //If not, save the position of the rightmost 1
           else {
               for(int i = m_previous_position; i < m_difference.cols - 1; i++) {
                   //If there is a 1 in the right interval of the row, the gesture is complete
                   if(m_difference.at<uchar>(i) == 1 && i >= (m_difference.cols - interval) ) {
                       //Emit the signal (parameter +1 means a gesture to the right)
                       emit gestureDetected(1);
                       //Reset status, previous position and previous frame and break
                       m_status = NOTHING;
                       m_previous_position = 0;
                       //m_previous_frame.operator=(cv::Scalar(255));
                       break;
                   }
                   //Else save the new previous position
                   if(m_difference.at<uchar>(i) == 1 && m_difference.at<uchar>(i+1) == 0)
                       m_previous_position = i;
               }
           }
       }
       break;

       //If an assumed gesture to the left is in process
       case LEFT:
           {
               bool continued = false;
               //Start at the previous position of the leftmost 1 and check if there is a new 1 to the left
               for(int i = m_previous_position; i >= (m_previous_position - interval) && i >= 0; i--) {
                   if(m_difference.at<uchar>(i) == 1)
                       continued = true;
               }
               //If no 1 was found, change status to NOTHING
               if(!continued) {
                   m_status = NOTHING;
                   m_previous_position = 0;
               }
               //Else keep the status and check if the gesture is finished
               //If not, save the position of the leftmost 1
               else {
                   for(int i = m_previous_position; i >= 1; i--) {
                       //If there is a 1 in the left interval of the row, the gesture is complete
                       if(m_difference.at<uchar>(i) == 1 && i <= interval) {
                           //Emit the signal (parameter -1 means a gesture to the left)
                           emit gestureDetected(-1);
                           //Reset status, previous position and previous frame and break
                           m_status = NOTHING;
                           m_previous_position = 0;
                           //m_previous_frame.operator=(cv::Scalar(255));
                           break;
                       }
                       //Else save the new previous position
                       if(m_difference.at<uchar>(i) == 1 && m_difference.at<uchar>(i-1) == 0)
                           m_previous_position = i;
                   }
               }
           }
           break;
    }

    //qDebug() << "CameraProcessor:process() finished";
    m_busy = false;
}
