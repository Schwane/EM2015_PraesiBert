/*
 * CameraProcessor.cpp
 *
 *  Created on: 15.06.2015
 *      Author: Inga
 */

#include "include/CameraProcessor.hpp"

//Constructor
CameraProcessor::CameraProcessor(QObject* parent) :
        QObject(parent),
        m_handle(CAMERA_HANDLE_INVALID),
        m_frametype(CAMERA_FRAMETYPE_NV12),
        m_framerate(8.0),
        m_rotation(90),
        m_threshold(10),
        m_reset_counter(0),
        m_previous_position(0),
        m_status(NOTHING)
{
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
    m_resolution.width = 288;
    m_resolution.height = 512;
    qDebug() << "CameraProcessor::startVf(): m_resolution set to " << m_resolution.width << " x "
            << m_resolution.height;
    //Set the framerate
    setFramerate();
    //Set the rotation
    setRotation();
    qDebug() << "CameraProcessor::startVf(): m_framerate set to " << m_framerate;

    //Set properties for the viewfinder (separately for more precise errors)

    /*
    //window
    // find the ForeignWindowControl (defined in main.qml) for the viewfinder
    bb::cascades::AbstractPane* current_root = bb::cascades::Application::instance()->scene();
    bb::cascades::ForeignWindowControl* fwc = current_root->findChild<bb::cascades::ForeignWindowControl*>("vf_foreign_window");
    const char* win_groupid = fwc->windowGroup().toStdString().c_str();
    const char* win_id = fwc->windowId().toStdString().c_str();
    err = camera_set_vf_property(m_handle,
            CAMERA_IMGPROP_WIN_GROUPID, win_groupid,
            CAMERA_IMGPROP_WIN_ID, win_id);
    if (err != CAMERA_EOK) {
        emit error("Error: Could not set viewfinder property: window");
        return;
    }
    */

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
    //err = camera_start_viewfinder(m_handle, NULL, NULL, NULL);
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
    qDebug() << num << " supported resolutions found (width x height):";
    camera_res_t lowest = resolutions[0];
    qDebug() << resolutions[0].width << " x " << resolutions[0].height;
    for (unsigned int i = 1; i < num; i++) {
        qDebug() << resolutions[i].width << " x " << resolutions[i].height;
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

    //Check if the preferred rotation is supported, if not, set it to 0
    bool supported = false;
    qDebug() << num << " supported rotations:";
    for (unsigned int i = 0; i < num; i++) {
        qDebug() << rotations[i];
        if (m_rotation == rotations[i]) {
            supported = true;
            break;
        }
    }
    if(!supported) {
        m_rotation = rotations[0];
    }
}

//Initializes the matrices m_previous_frame, m_actual_frame, m_previous_difference and m_actual difference
//with the correct size and sets all elements to zero
void CameraProcessor::initMatrices()
{
    //Set number of rows and columns for all matrices
    //Since they should all only contain the middle row of a frame, number of rows is always 1
    unsigned int rows = 1;
    //Number of columns equals either height or width of the set resolution depending on the rotation
    unsigned int cols;
    if(m_rotation == 90 || m_rotation == 270) {
        cols = m_resolution.height;
    }
    else {
        cols = m_resolution.width;
    }

    m_actual_frame = cv::Mat(rows, cols, CV_8UC1, cv::Scalar(0));
    m_previous_frame = cv::Mat(rows, cols, CV_8UC1, cv::Scalar(0));
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
    //Set a pointer to the CameraProcessor instance (normally "this", but the viewfinder callback function needs to be static)
    CameraProcessor* inst = (CameraProcessor*) arg;

    unsigned int height = buffer->framedesc.nv12.height;
    unsigned int width = buffer->framedesc.nv12.width;

    //Check if height and width are equal to m_resolution
    unsigned int rotation = inst->m_rotation;
    if(rotation == 0 || rotation == 180) {
        if (height != inst->m_resolution.height || width != inst->m_resolution.width) {
            qDebug() << "CameraProcessor::viewfinder_callback(): height or width of the buffered image don't match m_resolution";
        }
    }
    if(rotation == 90 || rotation == 270) {
        if (height != inst->m_resolution.width || width != inst->m_resolution.height) {
            qDebug() << "CameraProcessor::viewfinder_callback(): height or width of the buffered image don't match m_resolution";
        }
    }

    //Create an OpenCv matrix from the buffer (deep copy)
    cv::Mat image = cv::Mat(height, width, CV_8UC1, (void *) buffer->framebuf,
            buffer->framedesc.nv12.stride).clone();

    //qDebug() << "Read buffer into cv::Mat, resolution is now: width = " << image.cols << "height = " << image.rows;

    /*//Reduce it to the middle row (this references still the same data)
    inst->m_actual_frame = image.row((int) height / 2);

    //Compare it to the previous frame
    inst->m_difference = inst->m_actual_frame - inst->m_previous_frame;

    //Save it as the new previous frame (deep copy)
    inst->m_previous_frame = inst->m_actual_frame.clone();

    //Check each pixel in the difference image if the change exceeds the threshold
    bool changed = false;
    for(int i = 0; i < inst->m_difference.cols; i++) {
        //If pixels are significantly darker than before, set them to 1
        if(inst->m_difference.at<int>(i) >= inst->m_threshold) {
            inst->m_difference.at<int>(i) = 1;
            changed = true;
        }
        //Else if pixels are significantly lighter, set them to -1
        else if(inst->m_difference.at<int>(i) <= -(inst->m_threshold)) {
            inst->m_difference.at<int>(i) = -1;
            changed = true;
        }
        //Else there was no significant change - set those pixels to 0
        else {
            inst->m_difference.at<int>(i) = 0;
        }
    }

    //If there was no change detected, check the status, maybe change it, and return
    if(!changed) {
        //If the status is LEFT or RIGHT, it should only be kept for 2 frames without changes
        if(inst->m_status != NOTHING) {
            if(inst->m_reset_counter >= 2) {
                inst->m_status = NOTHING;
                inst->m_reset_counter = 0;
            }
            else {
                inst->m_reset_counter++;
            }
        }
        return;
    }
    //Else analyse the changes to the previous frame that have occurred

    //The interval serves the following functions:
    // - One interval from the start of the row counts as "left"
    // - One interval from the end of the row counts as "right"
    // - One interval from a previous position of an object counts as "continuous move"
    //TODO: Set to 5% of the total width, maybe another value is better
    unsigned int interval = inst->m_difference.cols / 20;

    //Depending on the state (NOTHING, LEFT or RIGHT)
    switch (inst->m_status) {

        //If there were no prior indications of any gesture
        case NOTHING:
            {
                bool left = false;
                bool right = false;
                //For each pixel
                for(int i = 0; i < inst->m_difference.cols; i++) {
                    //If it's 1, there was an object/movement detected
                    if(inst->m_difference.at<int>(i) == 1) {
                        //If the 1 is in the left part, note that
                        if((unsigned int)i < interval)
                            left = true;
                        //If in the right part, note that
                        if(i > (inst->m_difference.cols - 1 - interval) )
                            right = true;
                    }
                }

                //If both left and right are true, something is odd and the status should not change
                if(left && right) {
                    return;
                }

                //Else set status to LEFT or RIGHT
                if(left) {
                    inst->m_status = RIGHT;
                }
                if(right) {
                    inst->m_status = LEFT;
                }

                //If there are indications of a gesture, save the index of the leftmost or rightmost 1
                switch(inst->m_status) {
                    case RIGHT:
                        for(int i = 0; i < inst->m_difference.cols - 1; i++) {
                            if(inst->m_difference.at<int>(i) == 1 && inst->m_difference.at<int>(i+1) == 0)
                                inst->m_previous_position = i;
                        }
                        break;
                    case LEFT:
                        for(int i = inst->m_difference.cols - 1; i >= 1; i--) {
                            if(inst->m_difference.at<int>(i) == 1 && inst->m_difference.at<int>(i-1) == 0)
                                inst->m_previous_position = i;
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
                for(unsigned int i = inst->m_previous_position; i <= (inst->m_previous_position + interval); i++) {
                    if(inst->m_difference.at<int>(i) == 1)
                        continued = true;
                }
                //If no 1 was found, change status to NOTHING
                if(!continued) {
                    inst->m_status = NOTHING;
                    inst->m_previous_position = 0;
                }
                //Else keep the status and check if the gesture is finished
                //If not, save the position of the rightmost 1
                else {
                    for(int i = inst->m_previous_position; i < inst->m_difference.cols - 1; i++) {
                        //If there is a 1 in the right interval of the row, the gesture is complete
                        if(inst->m_difference.at<int>(i) == 1 && i >= (inst->m_difference.cols - interval) ) {
                            //Emit the signal (parameter +1 means a gesture to the right)
                            inst->gestureDetected(1);
                            //Reset status, previous position and previous frame and break
                            inst->m_status = NOTHING;
                            inst->m_previous_position = 0;
                            inst->m_previous_frame.operator=(cv::Scalar(0));
                            break;
                        }
                        //Else save the new previous position
                        if(inst->m_difference.at<int>(i) == 1 && inst->m_difference.at<int>(i+1) == 0)
                            inst->m_previous_position = i;
                    }
                }
            }
            break;

        //If an assumed gesture to the left is in process
        case LEFT:
            {
                bool continued = false;
                //Start at the previous position of the leftmost 1 and check if there is a new 1 to the left
                for(unsigned int i = inst->m_previous_position; i >= (inst->m_previous_position - interval); i--) {
                    if(inst->m_difference.at<int>(i) == 1)
                        continued = true;
                }
                //If no 1 was found, change status to NOTHING
                if(!continued) {
                    inst->m_status = NOTHING;
                    inst->m_previous_position = 0;
                }
                //Else keep the status and check if the gesture is finished
                //If not, save the position of the leftmost 1
                else {
                    for(unsigned int i = inst->m_previous_position; i >= 1; i--) {
                        //If there is a 1 in the left interval of the row, the gesture is complete
                        if(inst->m_difference.at<int>(i) == 1 && i <= interval) {
                            //Emit the signal (parameter -1 means a gesture to the left)
                            inst->gestureDetected(-1);
                            //Reset status, previous position and previous frame and break
                            inst->m_status = NOTHING;
                            inst->m_previous_position = 0;
                            inst->m_previous_frame.operator=(cv::Scalar(0));
                            break;
                        }
                        //Else save the new previous position
                        if(inst->m_difference.at<int>(i) == 1 && inst->m_difference.at<int>(i-1) == 0)
                            inst->m_previous_position = i;
                    }
                }
            }
            break;
    }*/

}

