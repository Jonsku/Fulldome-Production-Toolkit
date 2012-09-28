/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include "mainwidget.h"
#include "variablecolorpalette.h"
#include "keyframe.h"
#include "curvevariable.h"
#include "cubemappreview.h"
#include "cubemaploader.h"
#include "curveeditor.h"
#include "timeline.h"
#include "iowidget.h"



#include <QSplitter>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QScrollArea>

#include <QSettings>


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent)
{
    //init color palette for variables
    VariableColorPalette palette(QColor(50,100,200));


    preview = new CubeMapPreview(500);
    loader = new CubeMapLoader();
    curveEditor = new CurveEditor(this);
    curveEditor->setViewScale(0.1,-1);
    setEditorRange(100);
    curveEditor->setSnap(1,0);
    curveEditor->setCurrentFrame(0);




    //create and init variables
    CurveVariable * pitch = new CurveVariable("Pitch");
    CurveVariable * yaw = new CurveVariable("Yaw");
    CurveVariable * roll = new CurveVariable("Roll");
    CurveVariable * zoom = new CurveVariable("Zoom");
    CurveVariable * fov = new CurveVariable("Fov",180);
    CurveVariable * frame = new CurveVariable("Cubemap",0);

    //add variables to key frames watcher
    KeyFrames * keyFrames = KeyFrames::getInstance();
    keyFrames->addVariable(pitch);
    keyFrames->addVariable(zoom);
    keyFrames->addVariable(roll);
    keyFrames->addVariable(fov);
    keyFrames->addVariable(yaw);
    keyFrames->addVariable(frame);

    curveEditor->addVariable(pitch);
    curveEditor->addVariable(yaw);
    curveEditor->addVariable(roll);
    curveEditor->addVariable(zoom);
    curveEditor->addVariable(fov);
    curveEditor->addVariable(frame);

    preview->setPitchVar(pitch);
    preview->setFovVar(fov);
    preview->setYawVar(yaw);
    preview->setRollVar(roll);
    preview->setZoomVar(zoom);

    loader->setFrameVar(frame);

    //setup time line
    timeLine = new TimeLine(this);

    //init IO module
    importExport = new IOWidget(loader, timeLine, this);

    //let the timeline know that a frame has been keyed/un keyed
    connect(pitch,SIGNAL(changed(float,float)), timeLine, SLOT(update()));
    connect(yaw,SIGNAL(changed(float,float)), timeLine, SLOT(update()));
    connect(roll,SIGNAL(changed(float,float)), timeLine, SLOT(update()));
    connect(zoom,SIGNAL(changed(float,float)), timeLine, SLOT(update()));
    connect(fov,SIGNAL(changed(float,float)), timeLine, SLOT(update()));
    connect(frame,SIGNAL(changed(float,float)), timeLine, SLOT(update()));

    //update preview when cube map changes
    connect(loader,SIGNAL(faceChanged(QImage *,CubeFace)),preview,SLOT(setFace(QImage *, CubeFace)));

    //tells the cube map loader to load the cube map corresponding to the new frame
    connect(timeLine,SIGNAL(frameChanged(float)),loader,SLOT(variablesValues(float)));

    //tells the preview to display a new frame
    connect(timeLine,SIGNAL(frameChanged(float)),preview,SLOT(variablesValues(float)));

    //update the frame cursor of the curve editor
    connect(timeLine,SIGNAL(frameChanged(float)),curveEditor,SLOT(setCurrentFrame(float)));

    //Change the range of frames displayed by the curvor editor when the number of frames changes
    connect(timeLine,SIGNAL(frameNumberChanged(int)), this, SLOT(setEditorRange(int)));

    //Refresh preview when curves have been edited
    connect(curveEditor,SIGNAL(curvesChanged(float, float)),preview,SLOT(refreshVariablesValues(float, float)));

    //connect(timeLine,SIGNAL(frameChanged(float)),preview,SLOT(refresh()));
    //stop playback if the user interacts with the preview
    connect(preview,SIGNAL(previewChanged()),timeLine,SLOT(stop()));


    QTabWidget * tabWidget = new QTabWidget(this);
    tabWidget->addTab(loader,"Cube&map");
    tabWidget->addTab(curveEditor,"Curve &Editor");
    tabWidget->addTab(importExport,"E&xport");


    QScrollArea * previewScroll = new QScrollArea(this);
    previewScroll->setWidget(preview);
    previewScroll->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QPalette pal = previewScroll->palette();
    pal.setColor(QPalette::Background,Qt::black);
    previewScroll->setPalette(pal);


    QScrollArea * tabScroll = new QScrollArea(this);
    tabScroll->setWidget(tabWidget);
    //so the content will expand if possible
    tabScroll->setWidgetResizable(true);


    QSplitter * splitterH = new QSplitter(Qt::Horizontal);
    splitterH->addWidget(previewScroll);

    splitterH->addWidget(tabScroll);
    //splitterH->addWidget(tabWidget);
    //splitterH->setStretchFactor(0,1);
    splitterH->setStretchFactor(1,1);

    QSplitter * splitterV = new QSplitter(Qt::Vertical);
    splitterV->addWidget(splitterH);
    splitterV->addWidget(timeLine);
    splitterV->setStretchFactor(0,1);

    QGridLayout * gridLayout = new QGridLayout();

    gridLayout->addWidget(splitterV,0,0);

    setLayout(gridLayout);

    //load settings
    readSettings();
}

void MainWidget::setEditorRange(int frameNum){
    curveEditor->setSceneRect(QRectF(-10,600,(float)frameNum+2,-1200));
}

void MainWidget::saveAs(){
    //CurveVariable * cubemap, CurveVariable * fov, CurveVariable * pitch, CurveVariable * yaw, CurveVariable * roll, CurveVariable * zoom, int frameStart, int frameStop
    importExport->saveAs();
}

void MainWidget::save(){
    importExport->save();
}

void MainWidget::load(){
    int framesLoaded = importExport->load();
    if(framesLoaded > 0){
        timeLine->setFrameNumber(framesLoaded);
        timeLine->setCurrentFrame(0);
    }
}

void MainWidget::writeSettings()
{
    /*
    QSettings settings("j-u-t-t-u.net", "Cubemap Animator");

    settings.beginGroup("ProjectHistory");
    settings.setValue("lastProject",lastProject);
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
    */
}

void MainWidget::readSettings()
{
    QSettings settings("j-u-t-t-u.net", "Cubemap Animator");

    settings.beginGroup("ProjectHistory");
    QString lastFile = settings.value("lastProject","").toString();
    settings.endGroup();

    //load last saved file if any
    if(lastFile != ""){
        int framesLoaded = importExport->load(lastFile);
        if(framesLoaded > 0){
            timeLine->setFrameNumber(framesLoaded);
            timeLine->setCurrentFrame(0);
        }
    }
}

