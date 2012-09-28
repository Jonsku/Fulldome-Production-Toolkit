/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "curveitem.h"
#include "curvevariable.h"
#include <cmath>
#include <QPainter>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>

float CurveItem::activeCurveWidth = 5.0;
float CurveItem::inactiveCurveWidth = 2.0;

CurveItem::CurveItem(CurveVariable * variable, QGraphicsItem *parent, QObject* objparent) :
    QObject(objparent), QGraphicsPathItem(parent), inverseScale(1,1)
{
    this->variable = variable;
    previous = NULL;
    next = NULL;
    init();
}

CurveItem::CurveItem(CurveVariable * variable, CurveItem *previous, QGraphicsItem *parent, QObject* objparent) :
    QObject(objparent), QGraphicsPathItem(parent), inverseScale(1,1)
{
    this->variable = variable;
    previous->setNextItem(this);
    next = NULL;
    init();
}

void CurveItem::init(){
    p1 = new CurveHandleItem(this);
    p1->setData(0, QVariant(0));

    p2 = new CurveHandleItem(this,p1);
    p2->setData(0, QVariant(1));
    p2->setPos(20,0);
    p2->hide();

    p3 = new CurveHandleItem(this,p1);
    p3->setData(0, QVariant(2));
    p3->setPos(-20,0);
    p3->hide();

    setInterpolation(LinearInterpolation);
    active = false;
}

void CurveItem::noHandle(){
    p1 = NULL;
    p2 = NULL;
    p3 = NULL;
}

QPointF CurveItem::pos() const{
    if(p1 != NULL){
        return p1->pos();
    }
    //return default variable value
    return QPointF(0, variable->getDefaultValue() * variable->getViewScale().y());
}



void CurveItem::movePoint(int i){
    if(i == 0){
        if(previous != NULL && pos().x() < previous->pos().x()){
            //shift to the left
            CurveItem * p = previous;
            previous->setNextItem(next);
            if(p->previousItem()!=NULL){
                p->previousItem()->setNextItem(this);
            }else{
                previous = NULL;
            }
            setNextItem(p);
        }else if(next != NULL && pos().x() > next->pos().x()){
            //shift to right
            CurveItem * n = next;
            setNextItem(next->nextItem());

            if(previous!=NULL){
                previous->setNextItem(n);
            }else{
                n->setPreviousItem(NULL);
            }
            n->setNextItem(this);
        }

        updatePath();
        if(previous)
            previous->updatePath();

        //signal that the value might have changed

        float min;
        float max;
        getChangeRange(&min, &max);
        curveVariable()->valueChanged(min, max);

    }else if( i == 1 && interpolationType==BezierInterpolation){
        updatePath();
    }else if( i == 2 && previous != NULL && previous->interpolation() == BezierInterpolation){
        previous->updatePath();
    }
}

void CurveItem::getChangeRange(float * min, float * max){
    *min = (previous == NULL) ? -1 : previous->pos().x();
    *max = (next == NULL) ? -1 : next->pos().x();
}

QPointF CurveItem::constrainHandle(int h, QPointF handlePos){
    if(h == 1 && (handlePos.x() < 0)){
            handlePos.setX(0);
    }else if(h == 2 && previous != NULL && (handlePos.x() > 0)){
            handlePos.setX(0);
    }
    return handlePos;
}

void CurveItem::setPreviousItem(CurveItem *previous){

    this->previous = previous;
    showControlPoint(previous != NULL && previous->interpolation() == BezierInterpolation);
    updatePath();
}

void CurveItem::setNextItem(CurveItem *next){
    this->next = next;
    updatePath();
    if(next != NULL){
        if(interpolationType == BezierInterpolation){
            p2->show();
        }
        next->setPreviousItem(this);
    }else{
        p2->hide();
    }

}

void CurveItem::setScene(QGraphicsScene * scene){
    scene->addItem(this);
    //there might be no handles!
    if(p1 != NULL){
        scene->addItem(p1);
    }
    if(next != NULL){
        next->setScene(scene);
    }
}

void CurveItem::updatePath(){


    QPainterPath newPath;
    if(previous == NULL){
        newPath.moveTo(QPointF((scene() == NULL) ? -5000 : scene()->sceneRect().left(),pos().y()));
        newPath.lineTo(pos());
    }else{
        newPath.moveTo(pos());
    }

    if(next != NULL){

        switch(interpolationType){
        case LinearInterpolation:
            newPath.lineTo(next->pos());
            break;
        case ConstantInterpolation:
            newPath.lineTo(next->pos().x(),pos().y());
            newPath.lineTo(next->pos());
            break;
        case BezierInterpolation:
            newPath.cubicTo(
                        ( (p2->scenePos().x() < next->pos().x()) ? p2->scenePos() : QPointF(next->pos().x(), p2->scenePos().y()) ),
                        ( (next->controlPoint(true).x() > pos().x()) ? next->controlPoint(true) : QPointF(pos().x(), next->controlPoint(true).y()) ),
                        next->pos());
            break;
        }
    }else{
        newPath.lineTo(QPointF(scene() != NULL ? scene()->sceneRect().width() : 5000,pos().y()));
    }
    setPath(newPath);
}

void CurveItem::setInterpolation(Interpolation interpolation){
    interpolationType = interpolation;
    if(interpolation == BezierInterpolation){
        if(next != NULL){
            p2->show();
            next->showControlPoint();
        }

    }else{
        p2->hide();
        if(next != NULL){
            next->showControlPoint(false);
        }
    }
    // showControlPoint(previous != NULL && previous->interpolation() == BezierInterpolation);
    updatePath();
}

void CurveItem::showControlPoint(bool show){
    p3->setVisible(show);
}

CurveItem * CurveItem::createCurve(QPointF p){
    CurveItem * curve = new CurveItem(variable);

    curve->setScene(scene());
    curve->activateCurve(variable->selected(),this);
    curve->setColor(color);
    curve->setPen(pen());

    curve->counterSceneScale( QPointF(1.0/inverseScale.x(), 1.0/inverseScale.y()) );
    //connect the new curve
    connect(scene(), SIGNAL(sceneRectChanged(QRectF)), curve, SLOT(updatePath()));
    connect(scene(), SIGNAL(scaleChanged(QPointF)), curve, SLOT(counterSceneScale(QPointF)));
    curve->setInterpolation(interpolation());
    curve->setPos(p);

    //qDebug() << variable->name() << ", new curve " << curve->pos();
    return curve;
}

CurveItem * CurveItem::addSegment(QPointF p){
    if(p1 == NULL){
        //default curve needs to be replaced
        CurveItem * curve = createCurve(p);
        variable->setCurve(curve);
        scene()->update();
        scene()->removeItem(this);
        return curve;
    }

    if(p.x() == pos().x()){
        setPos(p);
        return this;
    }else if(p.x() < pos().x()){
        if(previous == NULL){
            //qDebug() << "prepend node";
            CurveItem * curve = createCurve(p);
            curve->setNextItem(this);
            scene()->update();
            return curve;
        }else{
           return previous->addSegment(p);
        }
    }else{

        if(next == NULL){
            //qDebug() << "append node";
            CurveItem * curve = createCurve(p);
            setNextItem(curve);
            scene()->update();
            return curve;
        }else if(next->pos().x() > p.x()){
            //qDebug() << "insert node";
            CurveItem * curve = createCurve(p);
            curve->setNextItem(next);
            setNextItem(curve);
            scene()->update();
            return curve;
        }else{
           return next->addSegment(p);
        }
    }
}

void CurveItem::deleteSegment(){

    if(previous == NULL && next == NULL){ //last segment
        //use default curve
        variable->noCurve();
        variable->curve()->setScene(scene());
    }
    if(previous == NULL){
        //remove first
        if(next != NULL){
            next->setPreviousItem(NULL);
        }
    }else if(next == NULL){
        //remove last
        if(previous != NULL){
            previous->setNextItem(NULL);
        }
    }else{
        previous->setNextItem(next);
    }
    previous = NULL;
    next = NULL;
    scene()->removeItem(p1);
    scene()->removeItem(p2);
    scene()->removeItem(p3);
    scene()->removeItem(this);
    p1 = NULL;
    p2 = NULL;
    p3 = NULL;

    //this = NULL;
}

void CurveItem::activateCurve(bool a, CurveItem * caller){
    //float neutralizeScale = 1.0/this->scale();
    /*if(p1 == NULL)
        return;
        */

    QPen curvePen = pen();
    if(active && !a){
        if(p1 != NULL){
            p2->hide();
            p3->hide();
        }
        curvePen.setWidthF(inactiveCurveWidth * inverseScale.x());
    }else if(!active && a){
        if(p1 != NULL){
            if(interpolationType == BezierInterpolation && next != NULL){
                p2->show();
            }
            if(previous != NULL && previous->interpolation() == BezierInterpolation){
                p3->show();
            }
        }
        curvePen.setWidthF(activeCurveWidth * inverseScale.x());
    }
    setPen(curvePen);
    active = a;

    if(previous != caller && previous != NULL){
        previous->activateCurve(a,this);
    }
    if(next != caller && next != NULL){
        next->activateCurve(a,this);
    }
}

void CurveItem::setVisible(bool visible, CurveItem * caller){
    QGraphicsPathItem::setVisible(visible);
    if(p1 == NULL)
        return;

    p1->setVisible(visible);
    if(previous != caller && previous != NULL){
        previous->setVisible(visible,this);
    }
    if(next != caller && next != NULL){
        next->setVisible(visible,this);
    }
}

void CurveItem::setEnabled(bool enabled, CurveItem * caller){

    QPen curvePen = pen();
    if(enabled){
        curvePen.setStyle(Qt::SolidLine);
    }else{
        curvePen.setStyle(Qt::DashLine);
    }
    setPen(curvePen);

    if(previous != caller && previous != NULL){
        previous->setEnabled(enabled,this);
    }
    if(next != caller && next != NULL){
        next->setEnabled(enabled,this);
    }
}

void CurveItem::setVisible(bool visible){
    setVisible(visible, NULL);
}

float CurveItem::valueAt(float coordX){
    if(coordX < pos().x()){
        if(previous == NULL){
            return pos().y();
        }else{
            return previous->valueAt(coordX);
        }
    }else if(next == NULL){
        return pos().y();
    }else if(coordX >= next->pos().x()){
        return next->valueAt(coordX);
    }else{
        if(interpolationType == CurveItem::ConstantInterpolation){
            return pos().y();
        }

        //normalised point position in the segment
        float u = (next->pos().x() - pos().x());
        u = (coordX - pos().x())/u;

        if(interpolationType == CurveItem::LinearInterpolation){
            return pos().y()*(1.0-u)+next->pos().y()*u;
        }else{
            //De Casteljau's Algorithm
            float resX = pos().x()-1;
            float resY = 0;
            u = 0;
            QPointF p2c = p2->scenePos();
            QPointF nextc = next->controlPoint(true);
            while(resX<coordX){
                QPointF p10(  pos().x() * (1.0F - u) + p2c.x() * u, pos().y() * (1.0F - u) + p2c.y() * u);
                QPointF p11(p2c.x() * (1.0F - u) + nextc.x() * u, p2c.y() * (1.0F - u) + nextc.y() * u);
                QPointF p12(nextc.x() * (1.0F - u) + next->pos().x() * u, nextc.y() * (1.0F - u) + next->pos().y() * u);

                QPointF p20(p10.x() * (1.0F-u) + p11.x() * u, p10.y() * (1.0F-u)+p11.y() * u);
                QPointF p21(p11.x() * (1.0F-u) + p12.x() * u, p11.y() * (1.0F-u)+p12.y() * u);
                resX =  p20.x() * (1.0F-u) + p21.x() * u;
                resY =  p20.y() * (1.0F-u) + p21.y() * u;
                u += 0.0001;
            }
            return resY;

        }
    }
}

CurveItem * CurveItem::curveAt(float coordX){
    if(coordX < pos().x()){
        if(previous == NULL){
            return this;
        }else{
            return previous->curveAt(coordX);
        }
    }else if(next == NULL){
        return this;
    }else if(coordX >= next->pos().x()){
        return next->curveAt(coordX);
    }else{
        return this;
    }
}

void CurveItem::setColor(QColor col, CurveItem * caller){
    this->color = col;
    QPen p = pen();
    p.setColor(color);
    setPen(p);

    if(previous != caller && previous != NULL){
        previous->setColor(col,this);
    }
    if(next != caller && next != NULL){
        next->setColor(col,this);
    }
}

void CurveItem::counterSceneScale(QPointF sceneScale){

    inverseScale.setX(1.0/sceneScale.x());
    inverseScale.setY(1.0/sceneScale.y());
    QPen p = pen();
    if(!variable->selected()){
        p.setWidthF(inactiveCurveWidth * inverseScale.x());
    }else{
        p.setWidthF(activeCurveWidth * inverseScale.x());
    }
    p.setColor(color);
    setPen(p);
    if(p1 == NULL)
        return;

    p1->counterSceneScale(sceneScale);
    p2->counterSceneScale(sceneScale);
    p3->counterSceneScale(sceneScale);
}

CurveItem * CurveItem::isCurveStart(float x, float tolerance, CurveItem * caller){
    if(p1 == NULL)
        return NULL;
    //qDebug() << "key at " << x << ", this x" << pos().x();
    //qDebug() << pos().x() - x;
    if(round(fabs(pos().x() - x)) <= tolerance/2.0){
       // qDebug() << "found!";
        return this;
    }else if(x < pos().x() && previous != caller && previous != NULL){
         return previous->isCurveStart(x, tolerance, this);
    }else if(next != caller && next != NULL){
        return next->isCurveStart(x, tolerance, this);
    }else{
        return NULL;
    }

}

QPointF CurveItem::getControlPoint1(){
    if(p2 == NULL){
        return pos();
    }
    return p2->pos();
}

QPointF CurveItem::getControlPoint2(){
    if(next == NULL){
        return pos();
    }
    return next->controlPoint();
}

void CurveItem::moveControlPoint1(QPointF moveTo){
    if(p2 == NULL){
        return;
    }
    p2->setPos(moveTo);
}

void CurveItem::moveControlPoint2(QPointF moveTo){
    if(next == NULL){
        return;
    }
    next->controlPointHandle()->setPos(moveTo);
}

void CurveItem::detach(CurveItem * caller){
    /*
    if(previous == NULL && next == NULL){ //last segment
        //use default curve
        variable->noCurve();
        variable->curve()->setScene(scene());
    }
    */
    if(p1 == NULL) //default curve
        return;
    scene()->removeItem(p1);
    scene()->removeItem(p2);
    scene()->removeItem(p3);
    scene()->removeItem(this);
    if(next != NULL && next != caller){
        next->detach(this);
    }
    if(previous != NULL && previous != caller){
        previous->detach(this);
    }

}
