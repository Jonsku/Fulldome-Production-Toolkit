/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "curvevariable.h"
#include "curvesview.h"
#include "curvecursor.h"

#include <QMouseEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>

#include <cmath>


CurvesView::CurvesView(QObject *parent) :
    QGraphicsScene(parent)
{
    selectionFlag = false;
    currentSelection = NULL;
    lastSelected = NULL;
    zoomStarted = false;
    zoomFactor = 1;

    snapX = 0;
    snapY = 0;

    //add, delete
    addAction = new QAction(tr("&Add"), this);
    connect(addAction, SIGNAL(triggered()), this, SLOT(addPoint()));

    deleteAction = new QAction(tr("&Delete"), this);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deletePoint()));

    //interpolation actions
    bezierAction = new QAction(tr("&Bezier"), this);
    bezierAction->setCheckable(true);
    connect(bezierAction, SIGNAL(triggered()), this, SLOT(setBezierInterpolation()));

    linearAction = new QAction(tr("&Linear"), this);
    linearAction->setCheckable(true);
    connect(linearAction, SIGNAL(triggered()), this, SLOT(setLinearInterpolation()));

    constantAction = new QAction(tr("&Constant"), this);
    constantAction->setCheckable(true);
    connect(constantAction, SIGNAL(triggered()), this, SLOT(setConstantInterpolation()));

    interpolationGroup = new QActionGroup(this);
    interpolationGroup->addAction(bezierAction);
    interpolationGroup->addAction(linearAction);
    interpolationGroup->addAction(constantAction);

    bezierAction->setChecked(true);

    connect(this, SIGNAL(selectionChanged()), this, SLOT(changeSelection())); 

    //display where is the current frame
    QPen frameCursorPen(Qt::SolidLine);
    frameCursorPen.setBrush(QBrush(qRgb(0,100,0), Qt::SolidPattern));
    frameCursorPen.setWidth(10);
    frameCursor = new CurveCursor(this,this,frameCursorPen);
    frameCursor->showCoord(false);

    /*
    display coordinates of points on curves,
    adding it to the scene after the frame cursor ensure that the coordinates
    will be shown on top
    */
    QPen curveCursorPen(Qt::DotLine);
    curveCursor = new CurveCursor(this,this,curveCursorPen);
    curveCursor->showHorizontal();
    connect(this, SIGNAL(scaleChanged(QPointF)), curveCursor, SLOT(counterSceneScale(QPointF)));

    connect(this, SIGNAL(scaleChanged(QPointF)), frameCursor, SLOT(counterSceneScale(QPointF)));
}

void CurvesView::addCurve(CurveItem * curve){
    curve->setScene(this);
    update();
    connect(this, SIGNAL(scaleChanged(QPointF)), curve, SLOT(counterSceneScale(QPointF)));
    connect(this, SIGNAL(sceneRectChanged(QRectF)), curve, SLOT(updatePath()));
}

void CurvesView::addPoint(){
    QModelIndexList indexList = itemSelectionModel->selection().indexes();

    //Ugly, would be more elegant to marshall CurveItem...
    CurveItem * toCurve = (CurveItem *)itemModel->data(indexList[0],Qt::UserRole+4).value<void *>();
    if(toCurve != NULL){
        /*CurveItem * curve = */
        toCurve->addSegment(anchorPoint);
        //connect(this, SIGNAL(sceneRectChanged(QRectF)), curve, SLOT(updatePath()));
        //connect(this, SIGNAL(scaleChanged(QPointF)), curve, SLOT(counterSceneScale(QPointF)));
    }
}

void CurvesView::deletePoint(){
    CurveHandleItem* handle = dynamic_cast<CurveHandleItem*> (selectedItems()[0]);
    handle->segment()->deleteSegment();
    //currentSelection = NULL;
}

void CurvesView::setBezierInterpolation(){
    CurveHandleItem* handle = dynamic_cast<CurveHandleItem*> (selectedItems()[0]);
    handle->segment()->setInterpolation(CurveItem::BezierInterpolation);
}

void CurvesView::setLinearInterpolation(){
    CurveHandleItem* handle = dynamic_cast<CurveHandleItem*> (selectedItems()[0]);
    handle->segment()->setInterpolation(CurveItem::LinearInterpolation);
}

void CurvesView::setConstantInterpolation(){
    CurveHandleItem* handle = dynamic_cast<CurveHandleItem*> (selectedItems()[0]);
    handle->segment()->setInterpolation(CurveItem::ConstantInterpolation);
}



void CurvesView::revertSelection(){
    qDebug() << "Revert selection";
    currentSelection = lastSelected;
    lastSelected = NULL;
    selectionFlag = false;
}

void CurvesView::changeSelection(){

    QList<QGraphicsItem *> selectedItems = this->selectedItems();

    qDebug() << selectedItems.size() << "items selected on change selection.";
    if(selectedItems.size() < 1){
        qDebug() << "Nothing selected";
        return;
    }

    if(selectedItems.size() == 1){
        CurveHandleItem* handle = dynamic_cast<CurveHandleItem*>(selectedItems[0]);
        if(handle == NULL){
            qDebug() << "this item is not handled";
            return;
        }

        if( !itemSelectionModel->isSelected(itemModel->index(handle->segment()->curveVariable())) ){
            itemSelectionModel->clear();
            itemSelectionModel->select(itemModel->index(handle->segment()->curveVariable()), QItemSelectionModel::Select);
        }
    }
}


void CurvesView::contextMenuEvent( QGraphicsSceneContextMenuEvent * event ){
    // for most widgets
    qDebug() << "contextMenuEvent";
    clearSelection();

    //snap to
    QPointF snapPos = event->scenePos();
    if(snapX != 0){
        snapPos.rx() = (int)round(snapPos.rx()/snapX)*snapX;
    }

    if(snapY != 0){
        snapPos.ry() = (int)round(snapPos.ry()/snapY)*snapY;
    }

    //mouseEvent->setScenePos(snapPos);
    anchorPoint = snapPos;
    QMenu menu;
    CurveHandleItem * handle = dynamic_cast<CurveHandleItem*> (itemAt(event->scenePos()));
    if(handle != NULL){
        qDebug() << "handle";

        handle->setSelected(true);
        if(handle->data(0).toInt() == 0){
            menu.addAction(deleteAction);
            QMenu * interpolationMenu = menu.addMenu(tr("&Interpolation"));
            switch(handle->segment()->interpolation()){
            case CurveItem::BezierInterpolation:
                bezierAction->setChecked(true);
                break;
            case CurveItem::LinearInterpolation:
                linearAction->setChecked(true);
                break;
            case CurveItem::ConstantInterpolation:
                constantAction->setChecked(true);
                break;
            }
            interpolationMenu->addAction(bezierAction);
            interpolationMenu->addAction(linearAction);
            interpolationMenu->addAction(constantAction);
        }
    }else if(itemSelectionModel->hasSelection()){
        qDebug() << "not handle";


        //if(currentSelection!=NULL){
        menu.addAction(addAction);
        //}
    }
    menu.exec(event->screenPos());
}


void CurvesView::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent ){
    if(mouseEvent->buttons() != Qt::NoButton && !selectedItems().empty()){
        if(selectedItems().size() > 1 || selectedItems().at(0)->data(0).toInt() == 0){
            //snap to
            QPointF snapPos = mouseEvent->scenePos();
            if(snapX != 0){
                snapPos.rx() = (int)round(snapPos.rx()/snapX)*snapX;
            }

            if(snapY != 0){
                snapPos.ry() = (int)round(snapPos.ry()/snapY)*snapY;
            }
            mouseEvent->setScenePos(snapPos);
        }
    }


    float x = mouseEvent->scenePos().x();
    float y = 0;

    if(itemSelectionModel->hasSelection()){
        QModelIndexList indexList = itemSelectionModel->selection().indexes();

        //TODO: Ugly, would be more elegant to marshall CurveItem...
        CurveItem * toCurve = (CurveItem *)itemModel->data(indexList[0],Qt::UserRole+4).value<void *>();
        if(toCurve != NULL && toCurve->isVisible()){
            y = toCurve->valueAt(x);
            curveCursor->show();
            curveCursor->setPos(x,y);
            //notify of update
            if(mouseEvent->buttons() != Qt::NoButton && !selectedItems().empty()){
                CurveHandleItem * handle = dynamic_cast<CurveHandleItem*> (itemAt(mouseEvent->scenePos()));
                if(handle != NULL){
                    float min;
                    float max;
                    handle->getChangeRange(&min, &max);
                    emit curvesChanged(min, max);
                }
            }
        }else{
            curveCursor->hide();
        }

    }else{
        curveCursor->hide();
    }




    //mouseEvent->accept();
    //qDebug() << "mouse move";
    QGraphicsScene::mouseMoveEvent(mouseEvent);



}

void CurvesView::zoomTo(float z){
    zoomFactor = z;
    if(zoomFactor<0.1){
        zoomFactor = 0.1;
    }else if(zoomFactor > 20){
        zoomFactor = 20;
    }
    emit zoomChanged(QPointF(zoomFactor,zoomFactor), zoomCenter);
}

void CurvesView::wheelEvent(QGraphicsSceneWheelEvent * event){
    if(event->modifiers().testFlag(Qt::ShiftModifier)){ // zoom only when SHIFT key pressed
        if(!zoomStarted){
            zoomCenter = event->scenePos();
            qDebug() << "Zoom center" << zoomCenter;
            zoomStarted = true;
        }
        if (event->delta() == 0) {
            event->ignore();
            return;
        }
        zoomFactor += (event->delta()>0)?0.05:-0.05;
        zoomTo(zoomFactor);
        /*
        if(zoomFactor<0.1){
            zoomFactor = 0.1;
        }else if(zoomFactor > 20){
            zoomFactor = 20;
        }
        */
        // qDebug() << "Zoom factor: " << zoomFactor;
        //qreal sc = pow(1.25, numSteps); // I use scale factor 1.25
        emit zoomChanged(QPointF(zoomFactor,zoomFactor), zoomCenter);

        event->accept();

    }else{
        zoomStarted = false;
        event->ignore();
    }

}

void CurvesView::changeScale(QPointF scaleFactor){
     emit scaleChanged(scaleFactor);
}

void CurvesView::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent){
    QList<QGraphicsItem*> selectedItems = this->selectedItems();
    //qDebug() << selectedItems.size() << "items selected on release.";

    zoomStarted = false;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void CurvesView::moveFrameCursor(float at){
    frameCursor->setPos(at,0);
}

