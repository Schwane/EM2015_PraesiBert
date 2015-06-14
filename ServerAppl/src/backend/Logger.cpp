/*
 * Logger.cpp
 *
 *  Created on: 14.06.2015
 *      Author: sebastian
 */

#include <QDate>
#include <QDir>
#include <QTime>

#include <src/backend/Logger.h>

ServerAppl::Logger serverLogObject;

namespace ServerAppl
{

    Logger::Logger()
    {
        QTime currentTime = QTime::currentTime();
        QDate currentDate = QDate::currentDate();
        QString timeString = QString("%1_%2_%3__%4h_%5min_%6sec")
                .arg(currentDate.day())
                .arg(currentDate.month())
                .arg(currentDate.year())
                .arg(currentTime.hour())
                .arg(currentTime.minute())
                .arg(currentTime.second());


        logFile = new QFile("logs/logFile__"
                + timeString
                + ".log"
                );

        if(logFile->open(QIODevice::ReadWrite))
        {
            logStream = new QTextStream(logFile);
            logStreamReady = TRUE;
        }
        else
        {
            logStreamReady = FALSE;
        }

#ifdef IS_DEBUG_VERSION
        debugLogFile = new QFile("logs/debugLogFile__"
                + timeString
                + ".log"
                );

        if(debugLogFile->open(QIODevice::ReadWrite))
        {
            debugLogStream = new QTextStream(debugLogFile);
            debugLogStreamReady = TRUE;
        }
        else
        {
            debugLogStreamReady = FALSE;
        }
#endif
    }

    Logger::~Logger()
    {
        logFile->close();
        delete(logStream);
        delete(logFile);

#ifdef IS_DEBUG_VERSION
        debugLogFile->close();
        delete(debugLogStream);
        delete(debugLogFile);
#endif
    }

    void Logger::writeLogEntry(QString entry)
    {
        if(logStreamReady)
        {
            *logStream << QTime::currentTime().toString("h:mm:ss.z") << " --> " << entry << endl;
        }
    }

#ifdef IS_DEBUG_VERSION
    void Logger::writeDebugLogEntry(const char file[], int line, QString entry)
    {
        if(debugLogStreamReady)
        {
            *debugLogStream << QTime::currentTime().toString("h:mm:ss.z") << " - " << file << ":" << line << " --> " << entry << endl;
        }
    }
#endif

} /* namespace ServerAppl */
