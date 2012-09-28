/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>
#include <QVariant>
#include "curvevariable.h"
#include "keyframe.h"
#include "variablecolorpalette.h"

CurveVariable::CurveVariable(QString variableName, float defaultValue, QObject *parent) :
    QObject(parent), viewScale(1,1)
{
    this->defaultValue = defaultValue;
    this->variableName = variableName;
    variableColor = VariableColorPalette::getNextColor().toRgb();
    //qDebug() << variableColor;
    isSelected = false;
    enabled = true;

    //no curve at first
    curveItem = NULL;
    defaultCurve = new CurveItem(this);
    defaultCurve->setColor(variableColor);
    //remove handles so it can't be grabbed
    defaultCurve->noHandle();
}

CurveItem * CurveVariable::setValueAt(float coordX, float coordY){

    if(curveItem != NULL){
        return curveItem->addSegment( QPointF( coordX * (1.0/viewScale.x()), coordY * (1.0/viewScale.y()) ) );
    }else{
        qDebug() << variableName << "curve item is null";
        qDebug() << "First key at " << coordX << ", " << coordY;
        return defaultCurve->addSegment( QPointF( coordX * (1.0/viewScale.x()), coordY * (1.0/viewScale.y()) ) );
    }
    emit valueSet(this, coordX, coordY);

}

void CurveVariable::removeKeyAt(float at) {
    CurveItem * keyCurve = curve()->isCurveStart( at * (1.0/viewScale.x()) );
    if(keyCurve != NULL)
        keyCurve->deleteSegment();
}

float CurveVariable::valueAt(float coordX){
    if(curveItem != NULL){
        return curveItem->valueAt( coordX * (1.0/viewScale.x()) ) * viewScale.y();
    }else{
        return defaultValue;
    }
}

void CurveVariable::showCurve(bool show ){
    if(curveItem != NULL){
        curveItem->setVisible(show);
    }else{
        defaultCurve->setVisible(show);
    }
}

bool CurveVariable::curveVisible(){
    if(curveItem != NULL){
        return curveItem->isVisible();
    }else{
        return defaultCurve->isVisible();
    }
}

void CurveVariable::enableVariable(bool enabled){
    this->enabled = enabled;
    if(curveItem != NULL){
        curveItem->setEnabled(enabled);
    }else{
        defaultCurve->setEnabled(enabled);
    }
}

void CurveVariable::setViewScale(QPointF newScale){
    if(curveItem != NULL){
        CurveItem * curve = curveItem;
        //start from the beginning
        while(curve->previousItem()!=NULL){
            curve = curve->previousItem();
        }
        while(curve!=NULL){
            QPointF newPos(curve->pos().x()/viewScale.x(), curve->pos().y()/viewScale.y());
            newPos.rx() *= newScale.x();
            newPos.ry() *= newScale.y();
            curve->setPos(newPos);
            curve = curve->nextItem();
        }
    }
    viewScale = newScale;

}

QColor CurveVariable::color(){
    return variableColor;
}

CurveItem * CurveVariable::curve(){
    if(curveItem != NULL){
        return curveItem;
    }
    return defaultCurve;
}

void CurveVariable::detachCurve(){
    if(curveItem != NULL){
        defaultCurve->setScene(curveItem->scene());
        curveItem->detach();
        curveItem = NULL;
    }
}

void CurveVariable::noCurve(){
    curveItem = NULL;
}

QVariantMap CurveVariable::getSerializedFrame(float at){
    QVariantMap frame;
    bool isKey = isKeyAt(at);
    frame["value"] = (double)valueAt(at);
    if(isKey){
        frame["key"] = true;
        CurveItem * c = curve()->curveAt(at * (1.0/viewScale.x()));
        frame["interpolation"] = c->interpolation();

        qDebug() << "Interpolation :" << c->interpolation() << frame["interpolation"].toInt();
        if(frame["interpolation"].toInt() == CurveItem::BezierInterpolation){
            QPointF cp1 = c->getControlPoint1();
            QPointF cp2 = c->getControlPoint2();
            frame["controlPoint1_X"] = cp1.x();
            frame["controlPoint1_Y"] = cp1.y();

            frame["controlPoint2_X"] = cp2.x();
            frame["controlPoint2_Y"] = cp2.y();
        }
    }
    return frame;
}

void CurveVariable::deserializeFrame(float at, QVariantMap frame){
    if(!frame["key"].toBool()){
        return;
    }
    float value = frame["value"].toFloat();
    CurveItem * c = setValueAt(at, value);
    c->setInterpolation((CurveItem::Interpolation)frame["interpolation"].toInt());
    if(c->interpolation() == CurveItem::BezierInterpolation){
        QPointF cp1(frame["controlPoint1_X"].toFloat(), frame["controlPoint1_Y"].toFloat() );
        QPointF cp2(frame["controlPoint2_X"].toFloat(), frame["controlPoint2_Y"].toFloat() );
        c->moveControlPoint1(cp1);
        c->moveControlPoint2(cp2);
    }
}



//------------------------

//IMPLEMENTATION OF CurveVariableListModel
CurveVariableListModel::CurveVariableListModel(QObject * parent) :
    QAbstractListModel(parent), viewScale(1,1)
{

}

//required
int CurveVariableListModel::rowCount ( const QModelIndex & parent) const{
    Q_UNUSED(parent);
    return variablesList.size();
}

QVariant CurveVariableListModel::data ( const QModelIndex & index, int role) const{
    switch(role){
    case Qt::DisplayRole:
        return variablesList[index.row()]->name();
    case Qt::UserRole:
        return variablesList[index.row()]->isEnabled();
    case Qt::UserRole+1:
        return variablesList[index.row()]->curveVisible();
    case Qt::UserRole+2:
        return variablesList[index.row()]->color();
    case Qt::UserRole+3:
        return variablesList[index.row()]->selected();
    case Qt::UserRole+4:
        //ugly but wil do for now
        return qVariantFromValue((void *) variablesList[index.row()]->curve());
    }
    return QVariant(QVariant::Invalid);
}

//optional, but required for edit
bool CurveVariableListModel::setData( const QModelIndex & index, const QVariant & value, int role){
    switch(role){
    case Qt::EditRole:
        return true;
    case Qt::UserRole:
        variablesList[index.row()]->enableVariable(value.toBool());
        return true;
    case Qt::UserRole+1:
        variablesList[index.row()]->showCurve(value.toBool());
        return true;
    case Qt::UserRole+3:
        variablesList[index.row()]->select(value.toBool());
        return true;
    }
    return false;
}

//custom, for insertion/deletion of variable
void CurveVariableListModel::addVariable(CurveVariable * variable){
    beginInsertRows(QModelIndex(),variablesList.size(),variablesList.size());
    variablesList.append(variable);
    variable->setViewScale(viewScale);
    connect(this,SIGNAL(scaleChanged(QPointF)), variable,SLOT(setViewScale(QPointF)));
    endInsertRows();
}

void CurveVariableListModel::removeVariable(CurveVariable * variable){
    int i = variablesList.indexOf(variable);
    beginRemoveRows(QModelIndex(),i,i);
    variable->setViewScale(QPointF(1,1));
    disconnect(variable);
    variablesList.remove(i);
    endRemoveRows();
}

QModelIndex CurveVariableListModel::index(CurveVariable * variable){
    int i = variablesList.indexOf(variable);
    return QAbstractListModel::index(i);
}

void CurveVariableListModel::setViewScale(QPointF viewScale){
    this->viewScale = viewScale;
    emit scaleChanged(viewScale);
}


