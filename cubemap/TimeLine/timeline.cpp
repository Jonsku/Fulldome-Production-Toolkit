/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "timeline.h"
#include "ruler.h"
#include "framesscene.h"
#include <QGridLayout>
#include <QTimeLine>
#include <QScrollBar>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QLabel>

TimeLine::TimeLine(QWidget *parent) :
    QWidget(parent)
{
    //load some icons
    QPixmap loadin;
    loadin.load(":/player/play_square_grey");
    playIcon.addPixmap(loadin);

    loadin.load(":/player/pause_square_grey");
    pauseIcon.addPixmap(loadin);

    loadin.load(":/player/forward_square_grey");
    forwardIcon.addPixmap(loadin);

    loadin.load(":/player/back_square_grey");
    backIcon.addPixmap(loadin);

    loadin.load(":/player/next_square_grey");
    toEndIcon.addPixmap(loadin);

    loadin.load(":/player/previous_square_grey");
    toStartIcon.addPixmap(loadin);

    //init default values
    fps = 24;

    timeLine = new QTimeLine(1000,this);
    timeLine->setCurveShape(QTimeLine::LinearCurve);

    framesScene = new FramesScene(this);

    scrollBar = new QScrollBar(Qt::Horizontal,this);
    scrollBar->setRange(0, frameNumber-framesScene->displayedFrame());
    scrollBar->setSingleStep(1);

    ruler = new Ruler(Qt::Horizontal,this);
    ruler->bottomToTop();
    ruler->setMaximumHeight(30);
    ruler->setMinimumHeight(30);
    ruler->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    ruler->setMajorMark(10);
    ruler->setMinorMark(5);

    playPauseButton = new QPushButton(playIcon,"",this);
    playPauseButton->setFlat(true);

    stepForwardButton = new QPushButton(forwardIcon,"",this);
    stepForwardButton->setAutoRepeat(true);
    stepForwardButton->setFlat(true);
    stepBackButton = new QPushButton(backIcon,"",this);
    stepBackButton->setAutoRepeat(true);
    stepBackButton->setFlat(true);

    toEndButton = new QPushButton(toEndIcon,"",this);
    toEndButton->setFlat(true);

    toStartButton = new QPushButton(toStartIcon,"",this);
    toStartButton->setFlat(true);

    fpsNumberBox = new QLineEdit("24");
    fpsNumberBox->setValidator(new QIntValidator(1,24,this));
    QLabel * fpsNumberLabel = new QLabel("Replay speed (Fps)", this);
    fpsNumberLabel->setBuddy(fpsNumberBox);

    frameNumberBox = new QLineEdit("100");
    frameNumberBox->setValidator(new QIntValidator(0,1000000,this));
    QLabel * frameNumberLabel = new QLabel("&Frames", this);
    frameNumberLabel->setBuddy(frameNumberBox);



    QGridLayout * gridLayout = new QGridLayout(this);
    gridLayout->addWidget(ruler,0,0,1,-1);

    gridLayout->addWidget(framesScene,1,0,1,-1);

    gridLayout->addWidget(scrollBar,2,0,1,-1);

    //buttons row
    gridLayout->setColumnStretch(0,1);
    gridLayout->addWidget(fpsNumberLabel,3,1,1,1);
    gridLayout->addWidget(fpsNumberBox,3,2,1,1);

    gridLayout->addWidget(toStartButton,3,3,1,1);
    gridLayout->addWidget(stepBackButton,3,4,1,1);
    gridLayout->addWidget(playPauseButton,3,5,1,1);
    gridLayout->addWidget(stepForwardButton,3,6,1,1);
    gridLayout->addWidget(toEndButton,3,7,1,1);
    gridLayout->addWidget(frameNumberBox,3,8,1,1);
    gridLayout->addWidget(frameNumberLabel,3,9,1,1);
    gridLayout->setColumnStretch(10,1);

    gridLayout->setRowStretch(5,1);


    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);
    setLayout(gridLayout);

    connect(scrollBar,SIGNAL(valueChanged(int)),framesScene,SLOT(setStartFrame(int)));
    connect(this, SIGNAL(frameNumberChanged(int)), framesScene,SLOT(setFrameNumber(int)));

    connect(framesScene, SIGNAL(rangeChanged(float,float)), ruler, SLOT(setRulerRange(float,float)));
    connect(framesScene, SIGNAL(rangeChanged(float,float)), this, SLOT(updateScrollbar()));
    connect(framesScene, SIGNAL(currentFrameChanged(int)), this, SLOT(setCurrentFrame(int)));

    //buttons
    connect(playPauseButton,SIGNAL(clicked()),this,SLOT(playPause()));
    connect(stepForwardButton,SIGNAL(clicked()),framesScene,SLOT(stepForward()));
    connect(stepBackButton,SIGNAL(clicked()),framesScene,SLOT(stepBack()));
    connect(toEndButton,SIGNAL(clicked()),framesScene,SLOT(toEnd()));
    connect(toStartButton,SIGNAL(clicked()),framesScene,SLOT(toStart()));

    //frame number box
    connect(frameNumberBox,SIGNAL(editingFinished()),this,SLOT(setFrameNumberFromBox()));

    //fps number box
    connect(fpsNumberBox,SIGNAL(editingFinished()),this,SLOT(setFpsFromBox()));

    //update
    connect(timeLine,SIGNAL(frameChanged(int)),framesScene,SLOT(setFrame(int)));
    connect(timeLine,SIGNAL(frameChanged(int)),this,SLOT(changeFrame(int)));
    connect(timeLine,SIGNAL(finished()),this,SLOT(stop()));
    setFrameNumber(100);

}

void TimeLine::setFrameNumber(int frmNum){
    frameNumber = frmNum;
    frameNumberBox->setText(QString::number(frmNum));
    updateScrollbar();
    updateTimeline();

    emit frameNumberChanged(frameNumber);
}

void TimeLine::setCurrentFrame(int f){
    if(timeLine->state() == QTimeLine::Running){
        playPause();
    }
    int frameToTime = (f+1) * (1000/fps);
    disconnect(timeLine,SIGNAL(frameChanged(int)),framesScene,0);
    disconnect(timeLine,SIGNAL(frameChanged(int)),this,0);
    timeLine->setCurrentTime(frameToTime);
    timeLine->resume();
    playPause();
    connect(timeLine,SIGNAL(frameChanged(int)),framesScene,SLOT(setFrame(int)));
    connect(timeLine,SIGNAL(frameChanged(int)),this,SLOT(changeFrame(int)));
    changeFrame(f);
}

void TimeLine::updateScrollbar(){
    float max = frameNumber-framesScene->displayedFrame();
    if(max > 0 && max < scrollBar->singleStep()){
        max += scrollBar->singleStep()-max;
    }
    scrollBar->setRange(0,max);
}

void TimeLine::setFps(int fs){
    fps = fs;
    if(fps < 1){
        fps = 1;
    }

    fpsNumberBox->setText(QString::number(fs));
    updateTimeline();
}

void TimeLine::setFpsFromBox(){
    setFps(fpsNumberBox->text().toInt());
}

void TimeLine::updateTimeline(){
    timeLine->setFrameRange(0,frameNumber-1);
    timeLine->setUpdateInterval(1000/fps);
    timeLine->setDuration(frameNumber * 1000/fps);
}

void TimeLine::playPause(){
    bool playing = timeLine->state() == QTimeLine::Running;
    if(playing){
        //timeLine->stop();
        qDebug() << "Pause";
        timeLine->setPaused(true);
        //playPauseButton->setText("Play");
        playPauseButton->setIcon(playIcon);
    }else{
        //playPauseButton->setText("Pause");
        playPauseButton->setIcon(pauseIcon);
        qDebug() << "Play";
        if(timeLine->state() == QTimeLine::Paused){
            qDebug() << "Was Paused on " <<  timeLine->currentFrame();
            //timeLine->setPaused(false); // Qt Bug: does not work
            timeLine->resume();
        }else{
            qDebug() << "Time line was stopped";
            timeLine->start();
        }

    }
}

void TimeLine::stop(){
    qDebug() << "Stop";
    if(timeLine->state() == QTimeLine::Running){
            playPause();
    }

    //playPauseButton->setText("Play");
    //playPauseButton->setIcon(playIcon);
}

void TimeLine::setFrameNumberFromBox(){
    setFrameNumber(frameNumberBox->text().toInt());
}


