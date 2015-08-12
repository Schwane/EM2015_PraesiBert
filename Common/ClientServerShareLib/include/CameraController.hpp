/*
 * CameraController.hpp
 *
 * Created on: 15.06.2015
 * Author: Inga Quatuor
 */

#ifndef CAMERACONTROLLER_HPP_
#define CAMERACONTROLLER_HPP_
#include <QObject>
#include <QThread>
#include "CameraProcessor.hpp"


/**
 * @class CameraController CameraController.hpp "Camera/CameraController.hpp"
 *
 * @brief CameraController class
 *
 * The CameraController class is part of the gesture control component. It works as
 * an interface for the rest of the application. When a CameraController object has
 * been created, it can be used to activate and deactivate the gesture control feature.
 */
class CameraController: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the CameraController class
     *
     * @param[in] parent Parent QObject that creates the CameraController object
     *
     * This constructor creates a new QThread (but does not start it yet) and checks
     *  if the device has a front camera. If not an error signal is emitted.
     */
    CameraController(QObject* parent = NULL);
    /**
     * @brief Deconstructor for the CameraController class
     */
     virtual ~CameraController();

public slots:
    /**
     * @brief Activates the gesture control feature
     *
     * Creates a CameraProcessor object, moves it to a separate thread and
     * calls its start()-function. This starts the scanning process for hand
     * gestures.
     */
    void start();
    /**
     * @brief Deactivates the gesture control feature
     *
     * Deletes the CameraProcessor object and stops the thread. Thus stops
     * scanning for hand gestures.
     */
     void stop();
    /**
     * @brief Gets called when a gesture has been detected
     *
     * @param[in] value Indicates which gesture has been detected: -1 = to the left, +1 = to the right
     *
     * If a gesture has been detected, this slot transfers the signal to the application
     */
    void onGestureDetected(int value);
    /**
     * @brief Gets called when an error has occurred
     *
     * @param[in] e QString with a description of the error
     *
     * If an error has occurred, this slot transfers the signal to the application
     */
    void onError(QString e);

signals:
    /**
     * @brief Emitted if an error has occured
     *
     * @param[out] e QString with a description of the error
     *
     * If something is not working (e. g. no front camera available), this signal is emitted.
     * If the CameraController receives an error signal from its CameraProcessor object,
     * this signal is used to transfer that error to the application.
     */
    void error(QString e);
    /**
     * @brief Emitted if a gesture is detected
     *
     * @param[out] value Signals which gesture has been detected: -1 = to the left, +1 = to the right
     *
     * If the CameraProcessor identifies a gesture, it emits a gestureDetected-Signal.
     * This signal here is used to tranfer that signal to the application.
     */
    void gestureDetected(int value);

private:
    //member variables
    QThread* m_thread;
    CameraProcessor* m_processor;

    //member functions
    bool hasFrontCamera();
};

#endif /* CAMERACONTROLLER_HPP_ */
