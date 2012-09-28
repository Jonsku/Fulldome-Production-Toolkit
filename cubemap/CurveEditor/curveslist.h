/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVESLIST_H
#define CURVESLIST_H

#include "curvelistitemwidget.h"
#include <QListView>
#include <QAbstractItemDelegate>


class CurveListItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    CurveListItemDelegate(QObject *parent = 0);

   void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
   QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

protected:
    CurveListItemWidget * widgetRef;
    bool	editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );
};

class CurvesList : public QListView
{
    Q_OBJECT
public:
    explicit CurvesList(QWidget *parent = 0);

signals:

public slots:

};

#endif // CURVESLIST_H
