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
    CameraProcessor(QObject* parent = 0);
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

signals:
    void gestureDetected(int value);
    void error(QString e);

private:
    //member variables
    camera_handle_t m_handle;
    camera_res_t m_resolution;
    double m_framerate;
    int m_threshold;
    cv::Mat m_previous_frame, m_actual_frame, m_difference;
    int m_previous_position;
    Status m_status;

    //member functions
    void setLowestResolution();
    void setFramerate();
    void openCamera();
    void closeCamera();
    void startVf();
    void stopVf();
    static void viewfinder_callback(camera_handle_t handle, camera_buffer_t* buffer, void* arg);

};

#endif /* CAMERAPROCESSOR_HPP_ */
