/*
 * CameraController.hpp
 *
 * Created on: 15.06.2015
 * Author: Inga Quatuor
 */

#include "CameraController.hpp"

//Constructor
CameraController::CameraController(QObject* parent) :
        QObject(parent),
        m_thread(new QThread(this)),
        m_processor(NULL)
{
    qDebug() << "CameraController object created";
    m_thread->setObjectName("ProcessorThread");
    if (!hasFrontCamera()) {
        emit error("Error: No front camera available");
    }
}

//Destructor
CameraController::~CameraController()
{
    stop();
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
    if (!m_thread->isRunning()) {
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
    }
    else {
        qDebug() << "CameraController::start(): Thread is already running";
    }
}

//Stops scanning
void CameraController::stop()
{
    qDebug() << "Slot CameraController::stop() called";
    //If a CameraProcessor instance was created, delete it
    if(m_processor != NULL) {
        //disconnect the signals
        disconnect(m_processor, SIGNAL(error(QString)), this, SLOT(onError(QString)));
        disconnect(m_thread, SIGNAL(started()), m_processor, SLOT(start()));
        disconnect(m_processor, SIGNAL(gestureDetected(int)), this, SLOT(onGestureDetected(int)));
        //call the destructor
        delete m_processor;
        m_processor = NULL;
        qDebug() << "CameraController::stop(): CameraProcessor object deleted";
    }
    else {
        qDebug() << "CameraController::stop(): CameraProcessor object was already deleted";
    }

    //If the thread is running, stop it
    if(m_thread->isRunning()) {
        m_thread->quit();
        qDebug() << "Slot CameraController::stop(): thread stopped";
    }
    else {
        qDebug() << "Slot CameraController::stop(): thread was already stopped";
    }
}

//If a gesture is identified by the CameraProcessor a signal is emitted
void CameraController::onGestureDetected(int value) {
    emit gestureDetected(value);
}

//If an error occurs, the stop() function gets called and an error signal is emitted
void CameraController::onError(QString e)
{
    stop();
    emit error(e);
}

