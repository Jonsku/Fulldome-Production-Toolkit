/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>


#include "renderthread.h"
#include "iowidget.h"

#include <QProcess>

RenderThread::RenderThread(QObject *parent):
    QThread(parent), encodingProgressRegExp("^frame=\\s*(\\d+)")
{

}

void RenderThread::run(){
    renderProcess = new QProcess();
    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    connect(renderProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(processError(QProcess::ProcessError)));
    connect(renderProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readyStandardOutput()));
    connect(renderProcess,SIGNAL(readyReadStandardError()),this,SLOT(readyErrorOutput()));
    renderProcess->start(program,arguments);
    //exec();
    if(!renderProcess->waitForStarted()){
        qDebug() << "Rendering failed to start";
        qDebug("Result: %s", renderProcess->readAll().data());
        listener->renderDone(false);
        return;
    }
    qDebug() << "Rendering started";

    if(!renderProcess->waitForFinished(-1)){
        qDebug() << "Rendering failed to finish";
        qDebug("Result: %s", renderProcess->readAll().data());
        listener->renderDone(false);
        return;
    }
    listener->renderDone(true);
    qDebug() << "Rendering done";
}

void RenderThread::readyStandardOutput(){
    //renderProcess->waitForReadyRead(-1);
    const char * data = renderProcess->readAllStandardOutput().constData();
    if(data != NULL){
        listener->displayInLog(QString(data));
    }
    //listener->displayInLog(QString(renderProcess->readAllStandardOutput().data()));
    //qDebug("Ffmpeg: %s",);
}

void RenderThread::readyErrorOutput(){

        const char * data = renderProcess->readAllStandardError().constData();
        if(data != NULL){
            listener->displayInLog(QString(data));
        }

    /*
    QString err(renderProcess->readAllStandardError());
    if(err.contains("frame=",Qt::CaseInsensitive)){
        //frame=  500 fps=326 q=31.0 Lsize=      19kB time=20.00 bitrate=   8.0kbits/s
        encodingProgressRegExp.indexIn(err,err.indexOf("frame=",Qt::CaseInsensitive), QRegExp::CaretAtOffset);
        //qDebug() << "Encoding progress" << encodingProgressRegExp.cap(1);
        emit encodingProgress(encodingProgressRegExp.cap(1).toInt());

    }
    */
}

void RenderThread::processError(QProcess::ProcessError error){
    qDebug() << error;
}

void RenderThread::quit(){
    if(renderProcess->state() == QProcess::Running){
        renderProcess->terminate();
    }
    QThread::quit();
}
