/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "curvecursor.h"
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsScene>


CurveCursor::CurveCursor(QGraphicsScene * scene, QObject *parent, QPen pen) :
    QObject(parent)
{
    inverseScale = QPointF(1,1);
    lastX = 0;
    lastY = 0;

    //save the pattern to be able to de-scale it
    dashPattern = pen.dashPattern();
    vertLine = new QGraphicsLineItem();
    vertLine->setPen(pen);

    horLine = new QGraphicsLineItem();
    horLine->setPen(pen);



    vertCoordText = new QGraphicsSimpleTextItem();
    vertCoordText->setBrush(QBrush(Qt::white));
    horCoordText = new QGraphicsSimpleTextItem();
    horCoordText->setBrush(QBrush(Qt::white));



    setScene(scene);
    showHor = false;
    showHorizontal(showHor);
}

void CurveCursor::setScene(QGraphicsScene * scene){
    scene->addItem(vertCoordText);
    scene->addItem(horCoordText);
    scene->addItem(vertLine);
    scene->addItem(horLine);
}

void CurveCursor::showHorizontal(bool show){
    showHor = show;
    if(showHor){
        horLine->show();
    }else{
        horLine->hide();
    }
}

void CurveCursor::setPos(float x, float y){
    lastX = x;
    lastY = y;
    vertCoordText->setText(QString::number( y*viewScale.y() ));
    vertCoordText->setPos(viewPortRect.left()+5*inverseScale.x(),y+5*inverseScale.y());


    horCoordText->setText(QString::number( x*viewScale.x() ));
    horCoordText->setPos(x+5*inverseScale.x(),viewPortRect.top()+5*inverseScale.y());

    vertLine->setLine(x,viewPortRect.top(),x,viewPortRect.bottom());
    horLine->setLine(viewPortRect.left(),y,viewPortRect.right(),y);


    //vertCoordText->scale(inverseScale, inverseScale);
    //horCoordText->scale(inverseScale, inverseScale);
}

QVector<qreal> CurveCursor::scaleDashPattern(float s){

    QVector<qreal> scaledPattern;
    for(int i=0;i<dashPattern.size();i++){
        if(dashPattern[i]*s < 1){
            i++;
            continue;
        }
        scaledPattern << dashPattern[i]*s;
        qDebug() << scaledPattern[i];
    }
    return scaledPattern;
}

void CurveCursor::counterSceneScale(QPointF sceneScale){
    inverseScale = QPointF(1.0/sceneScale.x(), 1.0/sceneScale.y());
    vertCoordText->resetTransform();
    vertCoordText->scale(inverseScale.x(), inverseScale.y());
    horCoordText->resetTransform();
    horCoordText->scale(inverseScale.x(), inverseScale.y());

    //de-scale the line pattern
    QPen linePen = vertLine->pen();
    linePen.setDashPattern( scaleDashPattern(inverseScale.y()) );
    vertLine->setPen(linePen);

    linePen = horLine->pen();
    linePen.setDashPattern( scaleDashPattern(inverseScale.x()) );
    horLine->setPen(linePen);
}

void CurveCursor::setZValue(qreal zV){
    vertLine->setZValue(zV);
    horLine->setZValue(zV);
    vertCoordText->setZValue(zV);
    horCoordText->setZValue(zV);
}

void CurveCursor::showCoord(bool show){
    vertCoordText->setVisible(show);
    horCoordText->setVisible(show);
}

void CurveCursor::show(bool s){
    vertCoordText->setVisible(s);
    horCoordText->setVisible(s);
    vertLine->setVisible(s);
    horLine->setVisible(s);
}

