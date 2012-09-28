/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QProcess>

class IOWidget;

class RenderThread : public QThread
{
    Q_OBJECT
public:
    explicit RenderThread(QObject *parent = 0);

        IOWidget * listener;
        QString program;
        QStringList arguments;
        QProcess * renderProcess;
        void setProcess(IOWidget * l, QString prog, QStringList args) { listener = l; program = prog; arguments = args; }
        void run();

protected:
        QRegExp encodingProgressRegExp;

signals:
        void encodingProgress(int);


public slots:
        void quit();
        void readyStandardOutput();
        void readyErrorOutput();
        void processError(QProcess::ProcessError error);
};

#endif // RENDERMANAGER_H
