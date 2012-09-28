/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>
#include "curveslist.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QCheckBox>
/*
Basically create a delegate inheriting QAbstractItemDelegate, with a reference to a widget which "looks" and "acts" like you want your nodes to do.
Override editorEvent in your delegate and pass the event to your widget, remembering to set it's data first and to adjust mouse position for the widget.

Then just use the sizeHint event to set data ( it will be called first every time your itemview needs something from your node) and render the widget to the painter in the paint event.

Presto, virtual itemview having only one "real" widget no matter how many nodes. Tested with 50k items, works beautifully.
  */
CurveListItemDelegate::CurveListItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent) {
    widgetRef = new CurveListItemWidget();
}

bool CurveListItemDelegate::editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ){
    qDebug() << "editor Event" << event;
    QMouseEvent * e1 = dynamic_cast<QMouseEvent*>(event);

    QPoint realPos = e1->pos()-option.rect.topLeft();

    if(e1->type()!=QEvent::MouseButtonRelease){

        //model->setData(index, !index.data(Qt::UserRole+3).toBool(), Qt::UserRole+3);
        qDebug() << "Not mouse release";
        return false;
    }

    QAbstractButton * checkBox = widgetRef->getBoxAt(realPos);

    if(checkBox == NULL){
        qDebug() << "No check box clicked";
        return false;
    }else{
        bool checked = false;
        if(checkBox->accessibleName() == "enableBox"){
            checked = !model->data(index,Qt::UserRole).toBool();
            model->setData(index, checked, Qt::UserRole);
        }else if(checkBox->accessibleName() == "visibleBox"){
            checked = !model->data(index,Qt::UserRole+1).toBool();
            model->setData(index, checked, Qt::UserRole+1);
        }else{
            qDebug() << "Unknown widget";
            return false;
        }
        checkBox->setChecked(checked);
    }
    return true;
}

void CurveListItemDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const{

    widgetRef->setGeometry(option.rect);
    QPixmap pixmap(widgetRef->size());
    QColor col = index.data(Qt::UserRole+2).value<QColor>();
    bool selected = index.data(Qt::UserRole+3).toBool();
    //qDebug() << "List item selected" << selected;

    QPalette palette = option.palette;
    //palette.setColor(QPalette::Base, col);
    //palette.setColor(QPalette::Window, col);
    if(selected)
        palette.setColor(QPalette::Background, col/*palette.color(QPalette::Highlight)*/);

    widgetRef->setPalette(palette);
    widgetRef->setColor(col);
    widgetRef->render(&pixmap);
    painter->drawPixmap(option.rect.topLeft(), pixmap);
}

QSize CurveListItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const{
    Q_UNUSED(option);
    widgetRef->setText(index.data().toString());
    widgetRef->setVarEnabled(index.data(Qt::UserRole).toBool());
    widgetRef->setVarVisible(index.data(Qt::UserRole+1).toBool());
    return widgetRef->sizeHint();
}



// IMPLEMENTATION CURVES LIST
CurvesList::CurvesList(QWidget *parent) :
    QListView(parent)
{
    setItemDelegate(new CurveListItemDelegate(this));
}
