 /*
 * CameraController.hpp
 *
 *  Created on: 15.06.2015
 *      Author: Inga
 */

#ifndef CAMERACONTROLLER_HPP_
#define CAMERACONTROLLER_HPP_
#include <QObject>
#include <QThread>
#include "CameraProcessor.hpp"

class CameraController: public QObject
{
    Q_OBJECT

public:
    CameraController(QObject* parent = NULL);
    virtual ~CameraController();

public slots:
    //Starts scanning for hand gestures in a separate thread
    void start();
    //Stops scanning
    void stop();
    //If a gesture has been detected, this slot transfers the signal to the application
    void onGestureDetected(int value);
    //Error handling
    void onError(QString e);

signals:
    //Emitted if something is not working (e. g. no front camera available)
    void error(QString e);
    //Emitted if a gesture is detected
    void gestureDetected(int value);

private:
    //member variables
    QThread* m_thread;
    CameraProcessor* m_processor;

    //member functions
    bool hasFrontCamera();
};

#endif /* CAMERACONTROLLER_HPP_ */
