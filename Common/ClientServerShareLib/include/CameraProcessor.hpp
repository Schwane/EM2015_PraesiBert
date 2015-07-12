/*
 * CameraProcessor.hpp
 *
 *  Created on: 15.06.2015
 *      Author: Inga
 */

#ifndef CAMERAPROCESSOR_HPP_
#define CAMERAPROCESSOR_HPP_
#include <stdlib.h>
#include <QObject>
#include <camera/camera_api.h>
#include <QDebug>
#include "opencv2/core/core.hpp"

class CameraProcessor: public QObject
{
    Q_OBJECT

public:
    CameraProcessor(QObject* parent = NULL);
    virtual ~CameraProcessor();

    //Enumeration that indicates the status during frame processing
    enum Status {
        NOTHING, //No indication of gesture
        RIGHT, //Maybe gesture to the right
        LEFT //Maybe gesture to the left
    };

public slots:
    void start();
    void stop();
    void process();


signals:
    void gestureDetected(int value);
    void error(QString e);
    void imageReady();

private:
    //member variables
    camera_handle_t m_handle;
    camera_frametype_t m_frametype;
    camera_res_t m_resolution;
    double m_framerate;
    unsigned int m_rotation;
    cv::Mat m_image;
    uchar m_threshold;
    cv::Mat m_actual_frame, m_previous_frame;
    cv::Mat m_difference;
    unsigned int m_reset_counter;
    int m_previous_position;
    Status m_status;
    double m_interval_percentage;
    static bool m_busy;
    static unsigned int m_not_processed_counter;

    //member functions
    void openCamera();
    void closeCamera();
    void startVf();
    void stopVf();
    void setFrametype();
    void setLowestResolution();
    void setFramerate();
    void setRotation();
    void initMatrices();
    static void viewfinder_callback(camera_handle_t handle, camera_buffer_t* buffer, void* arg);
};

#endif /* CAMERAPROCESSOR_HPP_ */
