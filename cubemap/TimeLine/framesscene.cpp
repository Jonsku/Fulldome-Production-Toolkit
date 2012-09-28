/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "keyframe.h"
#include "framesscene.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

FramesScene::FramesScene(QWidget *parent) :
    QLabel(parent)
{
    setFixedHeight(30);
    setMargin(0);
    currentFrame = 0;
    startFrame = 0;
    endFrame = 1;
}

void FramesScene::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(QPen(Qt::black));
    float frameWidth = 10;
    int f = startFrame;
    for(float i = 0 ; i<width();i+=frameWidth){
        painter.save();
        if(f >= endFrame){
            painter.setPen(QPen(Qt::gray));
            painter.setBrush(QBrush(Qt::gray));
        }else if(currentFrame == f){
            painter.setBrush(QBrush(Qt::green));
        }
        painter.drawRect(i,0,frameWidth,height()-1);

        if(KeyFrames::getInstance()->isKeyFrame(f)){
            //qDebug() << f << "is key frame";
            painter.drawEllipse(QPointF(i+(frameWidth/2.0), height()/2.0),frameWidth/4.0,frameWidth/4.0);
        }

        painter.restore();
        f++;
    }
}

void FramesScene::setCurrentFrame(int f){
    if(f>=endFrame || f < 0){
        return;
    }
    if(f != currentFrame){
        currentFrame = f;
        emit currentFrameChanged(currentFrame);
        update();
    }
}

float FramesScene::displayedFrame(){
    float frameWidth = 10.0;
    return ((float)width())/frameWidth;
}

int FramesScene::getFrameAt(float x){
    float frameWidth = 10;
    int frm = startFrame+((int)ceilf(x/frameWidth)-1.0);
    if(frm>=endFrame){
        return endFrame-1;
    }
    return frm;
}

void FramesScene::mousePressEvent ( QMouseEvent * event ){
    if( (event->buttons() & Qt::LeftButton) == Qt::LeftButton ){
        setCurrentFrame( getFrameAt(event->x()) );
    }
}

void FramesScene::mouseReleaseEvent ( QMouseEvent * event ){
    QLabel::mouseReleaseEvent(event);
}

void FramesScene::mouseMoveEvent ( QMouseEvent * event ){
    if( (event->buttons() & Qt::LeftButton) == Qt::LeftButton ){
        setCurrentFrame( getFrameAt(event->x()) );
    }
}

void FramesScene::resizeEvent ( QResizeEvent * event ){
    Q_UNUSED(event);
    emit rangeChanged(startFrame, ((float)startFrame)+displayedFrame());
}

void FramesScene::setStartFrame(int f){
    qDebug() << "start frame" << f;
    startFrame = f;
    emit rangeChanged(startFrame, ((float)startFrame)+displayedFrame());
    update();
}

void FramesScene::setFrameNumber(int f){
    endFrame = f;
    update();
}

void FramesScene::stepForward(){
    setCurrentFrame(currentFrame+1);
}

void FramesScene::stepBack(){
    setCurrentFrame(currentFrame-1);
}

void FramesScene::toStart(){
    setCurrentFrame(0);
}

void FramesScene::toEnd(){
    setCurrentFrame(endFrame-1);
}

