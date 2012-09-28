/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVEITEM_H
#define CURVEITEM_H

#include <QDebug>
#include "curvehandleitem.h"
#include <QVector2D>
#include <QGraphicsObject>

class CurveVariable;

class CurveItem : public QObject, public QGraphicsPathItem
{

    Q_OBJECT
public:
    enum Interpolation{LinearInterpolation = 0, BezierInterpolation = 1, ConstantInterpolation = 2};

    explicit CurveItem(CurveVariable * variable, QGraphicsItem *parent = 0, QObject* objparent = 0);
    explicit CurveItem(CurveVariable * variable, CurveItem *previous, QGraphicsItem *parent = 0, QObject* objparent = 0);

    static float activeCurveWidth;
    static float inactiveCurveWidth;

    void setPos ( const QPointF & pos ) { p1->setPos(pos); }
    QPointF pos() const;

    CurveItem * previousItem() const { return previous; }
    CurveItem * nextItem() const { return next; }
    void setPreviousItem(CurveItem *previous);
    void setNextItem(CurveItem *next);

    CurveHandleItem * controlPointHandle() const { return p3; }
    QPointF controlPoint(bool sceneCoord = false) const { if(sceneCoord) return p3->scenePos(); else return p3->pos(); }
    void showControlPoint(bool show = true);
    void setScene(QGraphicsScene * scene);

    void setInterpolation(Interpolation interpolation);
    Interpolation interpolation() { return interpolationType; }

    void detach(CurveItem * caller = NULL);

    void movePoint(int point);
    QPointF constrainHandle(int h, QPointF pos);
    CurveItem * addSegment(QPointF point);
    void deleteSegment();

    bool isActiveCurve() const { return active; }
    void activateCurve(bool a = true, CurveItem * caller = NULL);

    float valueAt(float coordX);
    CurveItem * curveAt(float coordX);

    void setVisible(bool visible, CurveItem * caller);
    void setVisible(bool visible);

    void setColor(QColor col, CurveItem * caller = NULL);

    CurveVariable * curveVariable() const { return variable; }
    CurveItem * isCurveStart(float x, float tolerance = 0,  CurveItem * caller = NULL);
    void setEnabled(bool enabled) { setEnabled(enabled,NULL); }
    void setEnabled(bool enabled, CurveItem * caller);

    void moveControlPoint1(QPointF moveTo);
    void moveControlPoint2(QPointF moveTo);
    QPointF getControlPoint1();
    QPointF getControlPoint2();

    void getChangeRange(float * min, float * max);

protected:
    CurveVariable * variable;
    CurveItem * createCurve(QPointF p);
private:
    QPointF inverseScale;
    CurveItem * previous;
    CurveItem * next;
    CurveHandleItem * p1;
    CurveHandleItem * p2;
    CurveHandleItem * p3;
    QColor color;

    Interpolation interpolationType;
    bool active;
    void init();

signals:
  //  void positionChanged(QPointF);

public slots:
    void counterSceneScale(QPointF sceneScale);
    void updatePath();
    void noHandle();
};


#endif // CURVEITEM_H
