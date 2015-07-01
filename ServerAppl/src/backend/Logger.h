/*
 * Logger.h
 *
 *  Created on: 14.06.2015
 *      Author: sebastian
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>

#define IS_DEBUG_VERSION

namespace ServerAppl
{

    class Logger
    {
    public:
        Logger();
        virtual ~Logger();

        void writeLogEntry(QString entry);
#ifdef IS_DEBUG_VERSION
        void writeDebugLogEntry(const char file[], int line, QString entry);
#endif
    private:
        QFile * logFile;
        QTextStream * logStream;
        bool logStreamReady;
#ifdef IS_DEBUG_VERSION
        QFile * debugLogFile;
        QTextStream * debugLogStream;
        bool debugLogStreamReady;
#endif
    };

} /* namespace ServerAppl */

extern ServerAppl::Logger serverLogObject;

#define WRITE_LOG( msg ) serverLogObject.writeLogEntry(QString(msg));

#ifdef IS_DEBUG_VERSION
#define WRITE_DEBUG( msg ) serverLogObject.writeDebugLogEntry(__FILE__, __LINE__, QString(msg)); qDebug() << __FILE__ << ":" << __LINE__ << " --> " << msg;
#else
#define WRITE_DEBUG( msg )
#endif

#endif /* LOGGER_H_ */
