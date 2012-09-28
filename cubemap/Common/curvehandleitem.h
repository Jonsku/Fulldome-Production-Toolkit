/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVEHANDLEITEM_H
#define CURVEHANDLEITEM_H

class CurveItem;
#include <QGraphicsLineItem>

class CurveHandleItem : public QGraphicsLineItem
{
public:
    CurveHandleItem(CurveItem * segment, QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    QPainterPath shape() const;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    CurveItem * segment() const { return segmentItem;}
    void getChangeRange(float * min, float * max);
    void counterSceneScale(QPointF sceneScale);
    void toggleChangeNotification(bool b = true);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    CurveItem * segmentItem;
    QPointF inverseScale;

    float handleSize;
    QRectF * handleRect;
    QPolygonF * handleDiamond;


/*
signals:
    void moved();

public slots:
*/
};

#endif // CURVEHANDLEITEM_H
