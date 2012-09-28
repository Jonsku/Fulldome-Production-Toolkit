/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVELISTITEMWIDGET_H
#define CURVELISTITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

class CurveListItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CurveListItemWidget(QWidget *parent = 0);

    QAbstractButton * getBoxAt(QPoint pos);

    void setVarEnabled(bool enabled = true);
    void setVarVisible(bool visible = true);

    void setColor(QColor col);

protected:
    void paintEvent ( QPaintEvent * event );
    QColor color;
    QCheckBox * enableBox;
    QLabel * nameLabel;
    QIcon visibleIcon;
    QIcon hiddenIcon;
    QPushButton * visibleBox;
    //QCheckBox * visibleBox;

    //void mousePressEvent(QMouseEvent * event);
signals:

public slots:
    void setText(QString lab);

};

#endif // CURVELISTITEMWIDGET_H
