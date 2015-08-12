/*
 * CameraProcessor.hpp
 *
 * Created on: 15.06.2015
 * Author: Inga Quatuor
 */

#ifndef CAMERAPROCESSOR_HPP_
#define CAMERAPROCESSOR_HPP_
#include <stdlib.h>
#include <QObject>
#include <camera/camera_api.h>
#include <QDebug>
#include "opencv2/core/core.hpp"

/**
 * @class CameraProcessor CameraProcessor.hpp "Camera/CameraProcessor.hpp"
 *
 * @brief CameraProcessor class
 *
 * The CameraProcessor class is part of the gesture control component. When the gesture
 * control feature gets activated, an instance of this class is created. It then enables
 * the front camera unit and starts the viewfinder. Every frame gets analyzed and if a
 * gesture is detected, an according signal is emitted. When the gesture control feature
 * gets deactivated, this class stops scanning and frees the camera ressource.
 */
class CameraProcessor: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the CameraProcessor class
     *
     * @param[in] parent Parent QObject that creates the CameraProcessor object
     *
     * This constructor sets all member variables to default values and connects
     * the imageReady-signal with the process()-slot, which is used for analyzing
     * the frames.<br>
     * To optimize the performance, try changing the following parameters:<br>
     * <ul><li>m_framerate: With a framerate of 16 (or less) frames per second every frame gets reliably processed,
     * but maybe it could be set higher for better results.</li>
     * <li>m_threshold: The threshold determines when a change in pixel value is deemed significant.
     * The pixel values range from 255 (white) to 0 (black). Too small changes from one
     * frame to the next may be caused by shadows etc., so they should be ignored.</li>
     * <li>m_interval_percentage: Determines the percentage of a frame that counts as left or right (where
     * gestures start and finish) and the interval in which a gesture is regarded
     * continued from one frame to the next.</li></ul>
     */
    CameraProcessor(QObject* parent = NULL);
    /**
     * @brief Deconstructor for the CameraController class
     *
     * If the camera ressource is still in use, the stop()-procedure gets called.
     */
    virtual ~CameraProcessor();

    //Enumeration that indicates the status during frame processing
    enum Status {
        NOTHING, //No indication of gesture
        RIGHT, //Maybe gesture to the right
        LEFT //Maybe gesture to the left
    };

public slots:
    /**
     * @brief Starts the camera and the gesture detection
     *
     * First enables the front camera unit by calling openCamera(), then adjusts
     * the settings and starts the viewfinder by calling startVf().
     */
    void start();
    /**
     * @brief Stops the camera and the gesture detection
     *
     * First stops the viewfinder by calling stopVf(), then disables the front
     * camera unit by calling closeCamera().
     */
    void stop();
    /**
    * @brief Analyzes the actual frame for a possible gesture
    *
    * When a frame is available, the callback function viewfinder_callback() gets
    * called. If the control variable m_busy is not set, the signal imageReady() is
    * emitted and process() gets called to analyze the new image which is stored in
    * m_image.
    */
    void process();

signals:
    /**
     * @brief Emitted if a gesture is detected
     *
     * @param[out] value Signals which gesture has been detected: -1 = to the left, +1 = to the right
     *
     * If a gesture has been identified, this signal is emitted.
     */
    void gestureDetected(int value);
    /**
     * @brief Emitted if an error has occured
     *
     * @param[out] e QString with a description of the error
     *
     * If an error has occurred (e. g. it was not possible to set a specific resolution),
     * this signal is emitted.
     */
    void error(QString e);
    /**
     * @brief Emitted if a new image is ready for processing
     *
     * When a frame is available, the callback function viewfinder_callback() gets
     * called. If the control variable m_busy is not set, this signal is emitted
     */
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

    /**
     * @brief Opens the front camera unit
     */
    void openCamera();
    /**
     * @brief Closes the camera unit
     */
    void closeCamera();
    /**
     * @brief Adjusts the settings and starts the viewfinder
     *
     * Some settings are entered directly. For other setting there are
     * auxiliary functions that are called, to get the best supported
     * values. When all settings are made, the viewfinder is started
     * with the viewfinder_callback()-function as parameter.
     */
    void startVf();
    /**
     * @brief Stops the viewfinder
     */
    void stopVf();
    //void setFrametype();
    /**
     * @brief Sets the lowest supported resolution
     *
     * Gets the lowest resolution that is supported by the device and
     * stores it in m_resolution.
     */
    void setLowestResolution();
    /**
     * @brief Sets the framerate
     *
     * Checks if m_framerate is a supported framerate. If not,
     * sets m_framerate to the closest supported framerate.
     */
    void setFramerate();
    /**
     * @brief Sets the rotation
     *
     * Checks if m_rotation is a supported rotation. If not,
     * sets m_rotation to the default rotation of the device.
     */
    void setRotation();
    /**
     * @brief Initializes the matrices used for image processing
     *
     * Initializes the matrices m_previous_frame, m_actual_frame and m_difference
     * with the correct size
     */
    void initMatrices();
    /**
     * @brief Gets called whenever a new frame is available
     *
     * Whenever a viewfinder frame buffer is available, this callback function gets
     * called. If the control variable m_busy is not set, it transfers the buffered
     * frame to m_image, an open cv matrix, and emits the signal imageReady().
     */
    static void viewfinder_callback(camera_handle_t handle, camera_buffer_t* buffer, void* arg);
};

#endif /* CAMERAPROCESSOR_HPP_ */
