/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVEVARIABLE_H
#define CURVEVARIABLE_H


#include <QColor>
#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QVariantMap>
#include "curveitem.h"

class CurveVariable : public QObject
{
    Q_OBJECT
public:
    explicit CurveVariable(QString variableName, float defaultValue = 0, QObject *parent = 0);
    CurveItem * setValueAt(float coordX, float coordY);
    float getDefaultValue() const { return defaultValue; }
    float valueAt(float coordX);
    void showCurve(bool show = true);
    bool curveVisible();
    void enableVariable(bool enabled=true);
    bool isEnabled() const { return enabled; }
    void select(bool s = true) { isSelected = s; if(curveItem != NULL) curveItem->activateCurve(s);}
    bool selected() const { return isSelected; }
    void setCurve(CurveItem * curveItem) { this->curveItem = curveItem; }
    CurveItem * curve();
    QString name() const { return variableName; }
    QPointF getViewScale() const { return viewScale; }
    bool isKeyAt(float at) { return (curve()->isCurveStart( at * (1.0/viewScale.x()), 1.0/viewScale.x() ) != NULL); }
    void removeKeyAt(float at);

    void deserializeFrame(float at, QVariantMap frame);
    QVariantMap getSerializedFrame(float at);

    QColor color();
    void detachCurve();
    void noCurve();

protected:
    QPointF viewScale;
    CurveItem * curveItem;
    CurveItem * defaultCurve;
    float defaultValue;
    bool enabled;
    bool isSelected;
    QString variableName;
    QColor variableColor;

signals:
    //signal the value changed in the interval min < v < max
    void changed(float min, float max);
    //signal a key value was added
    void valueSet(CurveVariable *, float cX, float cY);

public slots:
    void setViewScale(QPointF newScale);
    void valueChanged(float min, float max) { min *= (min >= 0) ? (1.0/viewScale.x()) : 1; max *= (max >= 0) ? (1.0/viewScale.x()) : 1; emit changed(min, max); }
};

class CurveVariableListModel : public QAbstractListModel{
    Q_OBJECT
    public:
        explicit CurveVariableListModel(QObject * parent = 0);
        //required
        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

        //optional, but required for edit
        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
        Qt::ItemFlags flags ( const QModelIndex & index ) const { Q_UNUSED(index); return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable; }

        //custom, for insertion/deletion/retrieving of variable
        void addVariable(CurveVariable * variable);
        void removeVariable(CurveVariable * variable);
        QModelIndex index(CurveVariable * variable);

    protected:
        QVector<CurveVariable *> variablesList;
        QPointF viewScale;

    signals:
        void scaleChanged(QPointF scale);

    public slots:
        void setViewScale(QPointF viewScale);
};

#endif // CURVEVARIABLE_H
