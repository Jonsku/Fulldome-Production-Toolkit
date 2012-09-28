/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include "curveeditor.h"
#include "curvecursor.h"
#include "variablecolorpalette.h"
#include "ruler.h"
#include "curveslist.h"
#include <curvevariable.h>
#include <QScrollBar>
#include <QGridLayout>
#include <QStringListModel>
#include <QItemSelection>
#include <QModelIndex>
#include <cmath>


CurveEditor::CurveEditor(QWidget *parent) :
    QFrame(parent)
{
    positiveUp = true;
    scene = new CurvesView(this);
    // a gradient background


     QLinearGradient gradient(QPointF(0, 0), QPointF(0, 5000));
     gradient.setSpread(QGradient::PadSpread);
     gradient.setColorAt(0,qRgb(100, 100, 100));
     gradient.setColorAt(0.75,qRgb(230, 230, 230));

    connect(scene, SIGNAL(zoomChanged(QPointF,QPointF)), this, SLOT(zoom(QPointF,QPointF)));
    connect(scene, SIGNAL(curvesChanged(float, float)), this, SLOT(curvesHaveChanged(float, float)));

    list = new CurvesList(this);
    model = new CurveVariableListModel();

    //setup model and selection model to synchronize the variables, list and the graphics scene
    list->setModel(model);
    connect(scene, SIGNAL(selectionChanged()), list, SLOT(update()));
    connect(list->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(handleSelection(const QItemSelection &, const QItemSelection &)));
    connect(this,SIGNAL(viewScaleChanged(QPointF)),model,SLOT(setViewScale(QPointF)));
    connect(this,SIGNAL(viewScaleChanged(QPointF)),scene->getCursor(),SLOT(setViewScale(QPointF)));
    connect(this,SIGNAL(viewScaleChanged(QPointF)),scene->getFrameCursor(),SLOT(setViewScale(QPointF)));
    connect(this,SIGNAL(moveFrameCursorTo(float)),scene,SLOT(moveFrameCursor(float)));

    scene->setModel(model);
    scene->setSelectionModel(list->selectionModel());

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMouseTracking(true);
    view->installEventFilter(this);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setBackgroundBrush(gradient);
    view->horizontalScrollBar()->installEventFilter(this);
    view->verticalScrollBar()->installEventFilter(this);

    zoomSlider = new QSlider(Qt::Horizontal,this);
    zoomSlider->setRange(1,100);
    zoomSlider->setTickInterval(10);

    connect(zoomSlider, SIGNAL(sliderMoved(int)), this, SLOT(setZoomFromSlider(int)));

    //zoom

    zoom(QPointF(1,1),QPointF(0,0));

    connect(list, SIGNAL(clicked( const QModelIndex)), scene, SLOT(update()));
    topRuler = new Ruler(Qt::Horizontal,this);
    topRuler->setAutoMark();
    topRuler->bottomToTop();



    leftRuler = new Ruler(Qt::Vertical,this);
    leftRuler->setAutoMark();
    leftRuler->rightToLeft();



    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->addWidget(list,0,0,2,1);
    layout->addWidget(zoomSlider,2,0);
    layout->setColumnStretch(0,1);

    layout->addWidget(leftRuler,1,1,2,1);

    layout->addWidget(topRuler,0,2);
    layout->addWidget(view,1,2,-1,1);

    layout->setRowStretch(1,1);
    layout->setColumnStretch(2,4);
    //layout->setStretchFactor(view,2);
    setLayout(layout);

}

void CurveEditor::addVariable(CurveVariable * var){
    scene->addCurve(var->curve());
    //add curve to the model
    model->addVariable(var);
}

QRectF CurveEditor::getScaledViewRect(float scaleX, float scaleY){
    //calculate the rectangle representing the displayed area of the scene
    //qDebug() << "Viewport rect" << view->viewport()->rect();

    QPointF tl(view->horizontalScrollBar()->value(), view->verticalScrollBar()->value());
    QPointF br = tl + view->viewport()->rect().bottomRight();

    /*
    if(view->horizontalScrollBar()->value() == 0){
        tl.setX(-br.x()/2.0);
        br.setX(br.x()/2.0);
    }
*/

    if(view->verticalScrollBar()->value() == 0){
        tl.setY(-br.y()/2.0);
        br.setY(br.y()/2.0);
    }


    QMatrix mat = view->matrix().inverted();
    mat.scale(scaleX,scaleY);
    return mat.mapRect(QRectF(tl,br));
}

void CurveEditor::setRulers(){
    QRectF rect = getScaledViewRect(viewScale.x(), fabs(viewScale.y()));

    //qDebug() << "Set ruler" << rect;
    //update rulers

    topRuler->setRulerRange(rect.x(), rect.x()+rect.width());
    topRuler->update();
    //flip the ruler vertical ruler
    if(positiveUp){
        leftRuler->setRulerRange(-rect.y(),-rect.y()-rect.height());
    }else{
        leftRuler->setRulerRange(rect.y(), rect.y()+rect.height());
    }
    //leftRuler->setRulerRange(rect.y(), rect.y()+rect.height());
    leftRuler->update();
    rect = getScaledViewRect(1,1);
    scene->getCursor()->setRect(rect);
    scene->getFrameCursor()->setRect(rect);
    scene->update();
}

bool CurveEditor::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    Q_UNUSED(event);
    QRectF viewRect = getScaledViewRect(1,1);
    //qDebug() << "event filter";
    /* QMatrix mat;
    mat.scale(viewScale,viewScale);
    */
    QRectF sceneRect = scene->sceneRect();
    //qDebug() << "Scene rect" << sceneRect;
    //qDebug() << "View rect" << viewRect;

    if( abs(sceneRect.width()) < abs(viewRect.width()) ){
        float w = viewRect.width()-sceneRect.width();
        sceneRect.adjust(/*(-w/2.0f)/viewScale.rx()*/ 0, 0, (w)/viewScale.rx(), 0);

    }

    if( abs(sceneRect.height()) < abs(viewRect.height()) ){
        float h = viewRect.height()-sceneRect.height();
        sceneRect.adjust(0, -h/2.0f, 0, h/2.0f);
    }

    scene->setSceneRect(sceneRect);

    if(view->verticalScrollBar()->isVisible()){
        topRuler->setScrollbarMargin(view->verticalScrollBar()->size().width());
    }else{
        topRuler->setScrollbarMargin(0);
    }

    if(view->horizontalScrollBar()->isVisible()){
        leftRuler->setScrollbarMargin(view->horizontalScrollBar()->size().height());
    }else{
        leftRuler->setScrollbarMargin(0);
    }
    setRulers();
    return false;
}

void CurveEditor::zoom(QPointF factor, QPointF centerPoint)
{
   // factor.rx() *=  viewScale.x();
    //factor.ry() *=  viewScale.y();
    qDebug() << "zoom";
    view->resetTransform();
    view->scale(factor.x(), factor.y());
    view->centerOn(centerPoint);
    scene->changeScale(factor);
    view->update();
    //or else the signal will loop
    disconnect(zoomSlider,SIGNAL(sliderMoved(int)),this,SLOT(setZoomFromSlider(int)));
    zoomSlider->setValue( (int)( zoomSlider->minimum()+(factor.x()/(20.0f - 0.1f))*(float)(zoomSlider->maximum()-zoomSlider->minimum())  )  );
    //re-connect
    connect(zoomSlider, SIGNAL(sliderMoved(int)), this, SLOT(setZoomFromSlider(int)));
}

void CurveEditor::setZoomFromSlider(int v){

    float factor = 0.1 + ( (float)(v-1) )*((20.0f-0.1f)/(float)(zoomSlider->maximum()-zoomSlider->minimum()));
    qDebug() << "Zoom slider moved" << v << " to " << factor;
    scene->zoomTo(factor);
    //zoom(QPointF( factor, factor), viewRect.center());
}

void CurveEditor::handleSelection(const QItemSelection & selected, const QItemSelection & deselected){
    QModelIndexList indexList =  selected.indexes();
    //select
    for(int i=0;i<indexList.size();i++){
        list->model()->setData(((QModelIndex)indexList[i]), QVariant(true), Qt::UserRole+3);
    }
    //deselect
    indexList =  deselected.indexes();
    for(int i=0;i<indexList.size();i++){
        list->model()->setData(((QModelIndex)indexList[i]), QVariant(false), Qt::UserRole+3);
    }

}

void CurveEditor::setViewScale(float x, float y){
    viewScale.rx() = x;
    viewScale.ry() = y;
    emit viewScaleChanged(viewScale);
}

void CurveEditor::setSnap(float hSnap, float vSnap){
    scene->setSnap(hSnap * (1.0/viewScale.x()), vSnap * (1.0/viewScale.y()));
}

void CurveEditor::setSceneRect(QRectF rect){
    rect.setLeft(rect.left()/viewScale.rx());
    rect.setWidth(rect.width()/viewScale.rx());

//    rect.setTop(rect.top()/viewScale.ry());
//    rect.setHeight(rect.height()/viewScale.ry());
    scene->setSceneRect(rect);
}

void CurveEditor::setCurrentFrame(float f){
    emit moveFrameCursorTo(f/viewScale.rx());
}


