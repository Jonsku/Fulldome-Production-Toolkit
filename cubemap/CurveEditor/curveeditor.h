/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVEEDITOR_H
#define CURVEEDITOR_H



#include "curvesview.h"

#include <QFrame>
#include <QGraphicsView>


class Ruler;
class CurveVariable;
class CurvesList;
class CurveVariableListModel;
class QSlider;

class CurveEditor : public QFrame
{
    Q_OBJECT
public:
    explicit CurveEditor(QWidget *parent = 0);
    QRectF getScaledViewRect(float scaleX, float scaleY);

protected:
    bool eventFilter(QObject *object, QEvent *event);
    bool positiveUp;

private:
    QPointF viewScale;
    QGraphicsView * view;
    CurvesView * scene;
    CurvesList * list;
    CurveVariableListModel *model;
    Ruler * topRuler;
    Ruler * leftRuler;
    QSlider * zoomSlider;

signals:
    void viewScaleChanged(QPointF vScale);
    void curvesChanged(float min, float max);
    void moveFrameCursorTo(float);

public slots:
    void addVariable(CurveVariable * var);
    void setRulers();
    void handleSelection(const QItemSelection & selected, const QItemSelection & deselected);
    void zoom(QPointF factor, QPointF centerPoint);
    void setViewScale(float x, float y);
    void setViewScale(QPointF scale) { setViewScale(scale.x(), scale.y()); }
    void setSnap(float hSnap, float vSnap);
    void setSceneRect(QRectF rect);
    void curvesHaveChanged(float min, float max) { min *= (min >= 0) ? (viewScale.x()) : 1; max *= (max >= 0) ? (viewScale.x()) : 1; emit curvesChanged(min, max); }
    void setCurrentFrame(float f);
    void setZoomFromSlider(int v);
};

#endif // CURVEEDITOR_H
