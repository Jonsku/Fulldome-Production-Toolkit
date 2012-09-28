/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <Qdebug>

#include "curvehandleitem.h"
#include "curveitem.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>

CurveHandleItem::CurveHandleItem(CurveItem * segment, QGraphicsItem *parent) :
    QGraphicsLineItem(parent), inverseScale(1,1)
{
    handleSize = 10;

    handleRect = new QRectF(-handleSize/2, -handleSize/2, handleSize, handleSize);
    handleDiamond = new QPolygonF();
    *handleDiamond << QPointF(-handleSize/2.0,0) << QPointF(0,-handleSize/2.0) << QPointF(handleSize/2.0,0) << QPointF(0,handleSize/2.0) << QPointF(-handleSize/2.0,0);
    segmentItem = segment;

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | ItemSendsGeometryChanges);
    setZValue(999999);

}


QRectF CurveHandleItem::boundingRect() const{
    qreal extra = ((pen().width() + handleSize) / 2.0)*inverseScale.x();

    QRectF bRect(line().p1(), QSizeF(line().p2().x() - line().p1().x(),line().p2().y() - line().p1().y()));
    bRect = bRect.normalized().adjusted(-extra, -extra, extra, extra);
    return bRect;

}

QPainterPath CurveHandleItem::shape() const{
    QPainterPath path = QGraphicsLineItem::shape();
    QPointF lastPoint = path.pointAtPercent(1);
    if(data(0).toInt() == 0){
        path.addEllipse(lastPoint, (handleSize)*inverseScale.x(),(handleSize)*inverseScale.y());
        return path;
    }

    QMatrix mat;
    mat.scale(inverseScale.x(),inverseScale.y());

    if(data(0).toInt() == 1){
        path.addRect(mat.mapRect(*handleRect).translated(lastPoint));
    }else{
        path.addPolygon(mat.map(*handleDiamond).translated(lastPoint));
    }

    return path;
}

void CurveHandleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(widget);
    QPointF mappedPos = this->mapFromParent(pos());
   // painter->drawRect(boundingRect().translated(mappedPos));
    if(isSelected()){
        painter->setBrush(QBrush(option->palette.color(QPalette::Text), Qt::SolidPattern));
    }


    if(data(0).toInt() == 0){
        setLine(mappedPos.x()-handleSize/2, mappedPos.y(), mappedPos.x()+handleSize/2, mappedPos.y());
        painter->drawEllipse(mappedPos,(handleSize/2.0)*inverseScale.x(),(handleSize/2.0)*inverseScale.y());
        return;
    }

    QLineF newLine(mapFromParent(QPointF(0,0)), mappedPos);
    setLine(newLine);
    painter->drawLine(newLine);

    QMatrix mat;
    mat.scale(inverseScale.x(),inverseScale.y());
    if(data(0).toInt() == 1){
        painter->drawRect(mat.mapRect(*handleRect).translated(mappedPos));
    }else{
        painter->drawPolygon(mat.map(*handleDiamond).translated(mappedPos));
    }
}

QVariant CurveHandleItem::itemChange(GraphicsItemChange change, const QVariant &value){
    if(change == ItemPositionHasChanged){
      segment()->movePoint(data(0).toInt());
    }else if(change == ItemPositionChange){
        QPointF newPos = value.toPointF();
        if(data(0).toInt() != 0){
             newPos = segment()->constrainHandle(data(0).toInt(), newPos);
        }
        return QGraphicsLineItem::itemChange(change, newPos);
    }
    return  QGraphicsLineItem::itemChange(change, value);
}


void CurveHandleItem::toggleChangeNotification(bool b){
    setFlag(ItemSendsGeometryChanges, b);
}

void CurveHandleItem::counterSceneScale(QPointF sceneScale){
    inverseScale.setX(1.0/sceneScale.x());
    inverseScale.setY(1.0/sceneScale.y());
}

void CurveHandleItem::getChangeRange(float * min, float * max){
    segment()->getChangeRange(min, max);
    if(data(0).toInt() == 1){
        *min = segment()->pos().x();
    }else if(data(0).toInt() == 2){
        *max = segment()->pos().x();
    }
}
