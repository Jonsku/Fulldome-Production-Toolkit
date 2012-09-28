/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include "curveitem.h"
#include <QGraphicsScene>
#include <QtGui/QActionGroup>
#include <QtGui/QAction>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QMenu>

class CurveVariableListModel;
class CurveCursor;

class CurvesView : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CurvesView(QObject *parent = 0);
    void addCurve(CurveItem * curve);
    void setModel( CurveVariableListModel * model ) {itemModel = model;}
    void setSelectionModel( QItemSelectionModel * selectionModel ) {itemSelectionModel = selectionModel;}
    CurveCursor * getCursor() const {return curveCursor; }
    CurveCursor * getFrameCursor() const {return frameCursor; }

protected:
    void contextMenuEvent( QGraphicsSceneContextMenuEvent * contextMenuEvent );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    void wheelEvent(QGraphicsSceneWheelEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

    CurveHandleItem * lastSelected;
    CurveHandleItem * currentSelection;
    void revertSelection();

private:
    bool selectionFlag;
    bool zoomStarted;
    QPointF zoomCenter;
    float zoomFactor;
    QAction *addAction;
    QAction *deleteAction;

    QActionGroup *interpolationGroup;
    QAction *bezierAction;
    QAction *linearAction;
    QAction *constantAction;

    CurveCursor * frameCursor;
    CurveCursor * curveCursor;

    QPointF anchorPoint;

    CurveVariableListModel * itemModel;
    QItemSelectionModel * itemSelectionModel;
    QVector<CurveHandleItem *> selectedHandles;

    float snapX;
    float snapY;

signals:
    void scaleChanged(QPointF newScale);
    void zoomChanged(QPointF, QPointF);
    //
    void curvesChanged(float min, float max);

public slots:
    void changeSelection();

    void addPoint();
    void deletePoint();
    void setBezierInterpolation();
    void setLinearInterpolation();
    void setConstantInterpolation();
    void changeScale(QPointF scaleFactor);
    void zoomTo(float z);
    void setSnap(float sX, float sY){ snapX = sX; snapY = sY; }
    void moveFrameCursor(float at);

};

#endif // CURVESVIEW_H
