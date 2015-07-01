/*
 * CameraProcessor.cpp
 *
 *  Created on: 15.06.2015
 *      Author: Inga
 */

#include <include/CameraProcessor.hpp>

//Constructor
CameraProcessor::CameraProcessor(QObject* parent) :
        QObject(parent), m_handle(CAMERA_HANDLE_INVALID), m_framerate(16.0), m_threshold(128), m_previous_position(0), m_status(
                NOTHING)
{
    qDebug() << "CameraProcessor object created";
}

//Destructor
CameraProcessor::~CameraProcessor()
{

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
        //TODO: Fehlerbehandlung
    }
}

void CameraProcessor::closeCamera()
{
    qDebug() << "CameraProcessor::closeCamera() called";
    camera_close(m_handle);
}

void CameraProcessor::startVf()
{
    qDebug() << "CameraProcessor::startVf() called";
    camera_error_t err;
    //Set the viewfinder mode (maybe CAMERA_VFMODE_CONTINUOUS_BURST is better)
    err = camera_set_vf_mode(m_handle, CAMERA_VFMODE_PHOTO);
    if (err != CAMERA_EOK) {
        //TODO: Fehlerbehandlung
    }
    qDebug() << "CameraProcessor::startVf(): viewfinder mode set to CAMERA_VFMODE_PHOTO";
    //Get the lowest supported resolution
    setLowestResolution();
    qDebug() << "CameraProcessor::startVf(): resolution set to " << m_resolution.width << " x "
            << m_resolution.height;
    //Set the framerate
    setFramerate();
    qDebug() << "CameraProcessor::startVf(): framerate set to " << m_framerate;
    //Set properties for the viewfinder (needs testing)
    err = camera_set_vf_property(m_handle, CAMERA_IMGPROP_FORMAT, CAMERA_FRAMETYPE_GRAY8,
            CAMERA_IMGPROP_WIDTH, m_resolution.width, CAMERA_IMGPROP_HEIGHT, m_resolution.height,
            CAMERA_IMGPROP_FRAMERATE, m_framerate, CAMERA_IMGPROP_STABILIZATION, 1, //enabled
            CAMERA_IMGPROP_VARIABLEFRAMERATE, 0, //disabled
            CAMERA_IMGPROP_CREATEWINDOW, 0, //disabled
            CAMERA_IMGPROP_MAXFOV, 0); //must be disabled to support STABILIZATION
    if (err != CAMERA_EOK) {
        //TODO: Fehlerbehandlung
    }
    qDebug() << "CameraProcessor::startVf(): viewfinder properties set";
    //Start the viewfinder with the callback function process() as parameter
    err = camera_start_viewfinder(m_handle, &viewfinder_callback, NULL, this);
    if (err != CAMERA_EOK) {
        //TODO: Fehlerbehandlung
    }
    qDebug() << "CameraProcessor::startVf(): viewfinder started";
}

void CameraProcessor::setLowestResolution()
{
    camera_error_t err;

    //First call the function to get the supported viewfinder resolutions
    //in presizing mode to get the needed array size
    unsigned int num;
    err = camera_get_supported_vf_resolutions(m_handle, 0, &num, NULL);
    if (err != CAMERA_EOK) {
        //TODO: Fehlerbehandlung
    }
    camera_res_t resolutions[num];

    //Then call it again
    err = camera_get_supported_vf_resolutions(m_handle, num, &num, resolutions);
    if (err != CAMERA_EOK) {
        //TODO: Fehlerbehandlung
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
        //TODO: Fehlerbehandlung
    }
    double framerates[num];

    //Then call it again
    err = camera_get_supported_vf_framerates(m_handle, CAMERA_FRAMETYPE_UNSPECIFIED, num, &num,
            framerates, &maxmin);
    if (err != CAMERA_EOK) {
        //TODO: Fehlerbehandlung
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
        bool contained = false;

        for (unsigned int i = 0; i < num; i++) {
            //If m_framerate is a supported value, keep it and break the loop
            if (m_framerate == framerates[i]) {
                contained = true;
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
        if (!contained)
            m_framerate = framerates[indexMinDistance];
    }
}

void CameraProcessor::stopVf()
{
    qDebug() << "CameraProcessor::stopVf() called";
    camera_stop_viewfinder(m_handle);

}

void CameraProcessor::viewfinder_callback(camera_handle_t handle, camera_buffer_t* buffer,
        void* arg)
{
    CameraProcessor* inst = (CameraProcessor*) arg;
    unsigned int height = buffer->framedesc.gray8.height;
    unsigned int width = buffer->framedesc.gray8.width;

    //Check if height and width are equal to m_resolution
    if (height != inst->m_resolution.height || width != inst->m_resolution.width) {
        //TODO: Fehlerbehandlung
    }

    //Create an OpenCv matrix from the buffer
    cv::Mat image = cv::Mat(height, width, CV_8UC1, (void *) buffer->framebuf,
            buffer->framedesc.gray8.stride).clone();

    //Reduce it to the middle row
    inst->m_actual_frame = image.row((int) height / 2);

    //For every pixel in this row
    for (int i = 0; i < inst->m_actual_frame.cols; i++) {
        //If the value exceeds the threshold, set it to 1
        if (inst->m_actual_frame.at<int>(i) > inst->m_threshold)
            inst->m_actual_frame.at<int>(i) = 1;
        //Else set it to 0
        else
            inst->m_actual_frame.at<int>(i) = 0;
    }

    //Compare the array to the previous array to detect movement
    inst->m_difference = inst->m_actual_frame - inst->m_previous_frame;

    //Depending on the state (nothing, left or right)
    switch (inst->m_status) {

        //If no prior indications of any gesture
        case NOTHING:
            //If an moving object was detected in the left of the row (= first 10% of total width)
            for (int i = 0; i < (int) inst->m_difference.cols / 10; i++) {
                if (inst->m_difference.at<int>(i))
                    inst->m_status = RIGHT;
            }
            //If status has changed break
            if (inst->m_status != NOTHING)
                break;
            //Else if an object was detected on the right
            for (int i = inst->m_difference.cols - 1;
                    i > (int) inst->m_difference.cols - 1 - inst->m_difference.cols / 10; i--) {
                if (inst->m_difference.at<int>(i))
                    inst->m_status = LEFT;
            }
            //In any case break now
            break;

            //If prior indications of a gesture to the right
        case RIGHT:
            //TODO
            break;

            //If prior indications of a gesture to the left
        case LEFT:
            //TODO
            break;
    }

}

