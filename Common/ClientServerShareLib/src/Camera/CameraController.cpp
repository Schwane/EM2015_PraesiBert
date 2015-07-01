/*
 * CameraController.cpp
 *
 *  Created on: 15.06.2015
 *      Author: Inga
 */

#include <include/CameraController.hpp>

//Constructor
CameraController::CameraController(QObject* parent) :
        QObject(parent)
{
    qDebug() << "CameraController object created";
    m_thread = new QThread(this);
    m_started = false;
    //connect(this, SIGNAL(error(QString)), parent, SLOT(WHATEVER());
    if (!hasFrontCamera()) {
        emit error("No front camera available");
    }
}

//Destructor
CameraController::~CameraController()
{

}

//Checks if the device has a front camera
bool CameraController::hasFrontCamera()
{
    unsigned int num;
    camera_unit_t cams[CAMERA_UNIT_NUM_UNITS];
    camera_error_t err;

    err = camera_get_supported_cameras(CAMERA_UNIT_NUM_UNITS, &num, cams);
    if (err == CAMERA_EOK) {
        for (unsigned int i = 0; i < num; i++) {
            if (cams[i] == CAMERA_UNIT_FRONT)
                return true;
        }
    }
    return false;
}

//Starts scanning for hand gestures in a separate thread
void CameraController::start()
{
    qDebug() << "Slot CameraController::start() called";
    if (!m_started) {
        //Instantiate a CameraProcessor object (without parent so you can move it to a thread)
        m_processor = new CameraProcessor(NULL);

        //Move it to its own thread
        m_processor->moveToThread(m_thread);
        qDebug() << "CameraControllar::start(): CameraProcessor instance moved to separate thread";

        //Connect the necessary signals and slots

        //error in CameraProcessor -> onError in CameraController
        connect(m_processor, SIGNAL(error(QString)), this, SLOT(onError(QString)));
        //started in QThread -> start in CameraProcessor
        connect(m_thread, SIGNAL(started()), m_processor, SLOT(start()));
        //gestureDetected in CameraProcessor -> onGestureDetected in CameraController
        connect(m_processor, SIGNAL(gestureDetected(int)), this, SLOT(onGestureDetected(int)));

        qDebug() << "CameraControllar::start(): signals connected";

        //Start the thread with high priority
        m_thread->start(QThread::HighPriority);
        qDebug() << "CameraControllar::start(): thread started with high priority";

        //Set the control variable
        m_started = true;
    }
}

//Stops scanning
void CameraController::stop()
{
    qDebug() << "Slot CameraController::stop() called";
    //Stop the thread
    m_thread->quit();
    qDebug() << "Slot CameraController::stop(): thread stopped";
    //Set the control variable
    m_started = false;
}

void CameraController::onGestureDetected(int value) {
    //TODO
}

void CameraController::onError(QString e)
{
    emit error(e);
}

