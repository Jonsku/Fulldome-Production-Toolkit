/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include "mainwindow.h"
#include "mainwidget.h"
#include <QMenuBar>
#include <QMenu>
#include <QSettings>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    MainWidget * mainWidget = new MainWidget(this);
    setCentralWidget(mainWidget);

    //add menu
   QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
   fileMenu->addAction("Load",mainWidget,SLOT(load()));
   fileMenu->addAction("Save",mainWidget,SLOT(save()));
   fileMenu->addAction("Save as",mainWidget,SLOT(saveAs()));

   //apply settings
   readSettings();
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
 {
     //if (userReallyWantsToQuit()) {
         writeSettings();
         event->accept();
     //} else {
       //  event->ignore();
     //}
 }

void MainWindow::writeSettings()
{
    QSettings settings("j-u-t-t-u.net", "Cubemap Animator");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings("j-u-t-t-u.net", "Cubemap Animator");

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1080,740)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}
