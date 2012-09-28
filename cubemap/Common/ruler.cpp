/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include "ruler.h"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QSize>
#include <cmath>

Ruler::Ruler(QWidget *parent) :
    QWidget(parent)
{
    setOrientation(Qt::Horizontal);
    init();
}

Ruler::Ruler(Qt::Orientation orientation, QWidget *parent) :
    QWidget(parent)
{
    setOrientation(orientation);
    init();
}

void Ruler::init(){
    if(direction == Qt::Horizontal){
        setMinimumHeight(30);
    }else{
        setMinimumWidth(30);
    }
    lToR = true;
    start = 0;
    stop = 1.0;
    majorMark = 0.5;
    minorMark = 0.1;
    scrollbarMargin = 0;
    autoMark = false;
}

void Ruler::paintEvent( QPaintEvent * event ){
    if(parentWidget()==NULL){
        event->ignore();
        return;
    }

    //remove any specified margin
    QSize sizeMargin = size();
    if(scrollbarMargin != 0){
        if(direction == Qt::Horizontal){
            sizeMargin.setWidth(sizeMargin.width()-scrollbarMargin);
        }else{
            sizeMargin.setHeight(sizeMargin.height()-scrollbarMargin);
        }
    }

    QPainter painter(this);


    //draw a white background rectangle
    painter.setBrush(QBrush(palette().color(QPalette::Base)));
    QPen pen(palette().color(QPalette::Base));
    painter.setPen(pen);

    QRect paintRect = this->rect();
    paintRect.setSize(sizeMargin);
    painter.setClipRect(paintRect);
    painter.drawRect(paintRect);

    //apply text color for the pen
    pen.setColor(palette().color(QPalette::Text));
    pen.setWidth(1);
    painter.setPen(pen);

    //calculate mark line end points
    //base orientation horizontal topToBottom
    QPointF top(0,0); //start of marks
    QPointF bottom(0,0.8); //end of major marks
    if(!lToR){
        //swap top and bottom
        top.setY(1.0-bottom.y());
    }

    //float ratio = 1.0;
    float length = 1.0;
    if(direction == Qt::Horizontal){
        length = (float)sizeMargin.width();
        //  ratio = ((float)sizeMargin.width())/(float)abs(stop-start);
        top = top * ((float)sizeMargin.height());
        bottom = bottom * ((float)sizeMargin.height());
    }else{
        //ratio = ((float)sizeMargin.height())/(float)abs(stop-start);
        length = (float)sizeMargin.height();
        top = QPointF(top.y(), top.x());
        bottom = QPointF(bottom.y(), bottom.x());
        top = top * ((float)sizeMargin.width());
        bottom = bottom * ((float)sizeMargin.width());
    }
    /*
        if auto mark is on, calculate appropriate mark:
        major mark should be power of 10 : e.g. 0.001, 10, 100
        minor mark are half of that : e.g. 0.0005, 5, 50
    */
    double intPart;
    if(autoMark){
        double l = log10(abs(stop-start)/20.0);
        majorMark = pow(10, ceil(l));
        while(majorMark < abs(stop-start)/20.0){
            l+= 0.5;
            majorMark = pow(10, ceil(l));
        }
        minorMark = majorMark/2.0;
    }

    //calculate position of first major mark
    float majorMarkStep = (length*majorMark)/( (float)fabs(stop-start) );
    float fractPart = modf( ( ( start+majorMark*( (start<stop) ? 1.0 : 0 )) )/majorMark,&intPart);
    float v = ((float)intPart - ( (start<stop) ? 1.0 : 0 ) )*majorMark;
    float startStep = 0;
    if(start != 0){
        startStep = majorMarkStep * fractPart * ( (stop<start) ? -1.0 : 1.0 );
    }
    /*
    qDebug() << "--------";
    qDebug() << "Major mark" << majorMark;
    qDebug() << "First val" << v;
    qDebug() << "Major mark step" << majorMarkStep;
    qDebug() << "Start step" << startStep;
    qDebug() << "Fract part" << fractPart;
    qDebug() << "Int part" << intPart;
    qDebug() << "--------";
    */

    //how much to translate between lines
    QPointF step(majorMarkStep,0);

    //coordinate of the first mark
    QPointF p(top.x(),top.y());


    if(direction != Qt::Horizontal){
        step = QPointF(step.y(), step.x());
        p.setY(p.y()-startStep);
    }else{
        p.setX(p.x()-startStep);
    }

    //draw marks
    for(; (stop > start) ? v <= stop : v >= stop ; v += majorMark * ((stop > start) ? 1 : -1)){
        painter.drawLine(p,p+bottom);
        painter.save();
        painter.translate(p.x()+5,p.y()+bottom.y()*0.45);
        if(direction != Qt::Horizontal){
            painter.rotate(90);
        }
        painter.drawText(QPointF(0,0), QString::number(v));
        painter.restore();
        p += step;
    }

    //repeat previous steps for minor marks

    float minorMarkStep = (length*minorMark)/( (float)fabs(stop-start) );
    fractPart = modf( ( ( start+minorMark*( (start<stop) ? 1.0 : 0 )) )/minorMark,&intPart);
    v = ((float)intPart - ( (start<stop) ? 1.0 : 0 ) )*minorMark;
    startStep = 0;
    if(start != 0){
        startStep = minorMarkStep * fractPart * ( (stop<start) ? -1.0 : 1.0 );
    }


    step.setX(minorMarkStep);
    step.setY(0);


    //coordinate of the first mark
    p.setX(top.x());
    p.setY(top.y());

    if(direction != Qt::Horizontal){
        step = QPointF(step.y(), step.x());
        p.setY(p.y()-startStep);
    }else{
        p.setX(p.x()-startStep);
    }

    for(;(stop > start) ? v <= stop : v >= stop ; v += minorMark * ((stop > start) ? 1 : -1)){
        painter.drawLine((lToR) ? p : p+bottom, p+bottom*0.5);
        p += step;
    }
}

void Ruler::setStart(float s){
    start = s;
}

void Ruler::setStop(float s){
    stop = s;
}

void Ruler::setRulerRange(float s, float e) {
    setStart(s);
    setStop(e);
    update();
}

void Ruler::setMajorMark(float m){
    majorMark = m;
}

void Ruler::setMinorMark(float m){
    minorMark = m;
}

void Ruler::setScrollbarMargin(float margin){
    scrollbarMargin = margin;
}

void Ruler::setAutoMark(bool on){
    autoMark = on;
}

