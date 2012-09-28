/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>
#include "curvelistitemwidget.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>

CurveListItemWidget::CurveListItemWidget(QWidget *parent) :
    QWidget(parent)
{
    enableBox = new QCheckBox(this);
    enableBox->setAccessibleName("enableBox");
    enableBox->setMinimumSize(25,25);
    enableBox->setAutoFillBackground(false);

    enableBox->setGeometry(0,0,100,100);
    nameLabel = new QLabel(this);

    QPixmap icon;
    icon.load(":/curveEditor/icons/opened_eye.png");
    visibleIcon.addPixmap(icon);

    icon.load(":/curveEditor/icons/closed_eye.png");
    hiddenIcon.addPixmap(icon);

    visibleBox = new QPushButton(visibleIcon,"",this);
    visibleBox->setMinimumSize(25,25);
    //visibleBox = new QCheckBox(this);
    visibleBox->setFlat(true);
    visibleBox->setAccessibleName("visibleBox");

    QHBoxLayout *layout = new QHBoxLayout;

    layout->setMargin(0);
    layout->addWidget(enableBox);


    layout->setStretchFactor(enableBox,0);
    qDebug() << "Can align: " << layout->setAlignment(enableBox,Qt::AlignCenter);

    layout->addWidget(nameLabel);
    layout->setStretchFactor(nameLabel,1);
    layout->addWidget(visibleBox);
    layout->setStretchFactor(visibleBox,0);
    setLayout(layout);
}

QAbstractButton * CurveListItemWidget::getBoxAt(QPoint pos){
    QAbstractButton * w = dynamic_cast<QAbstractButton*>(this->childAt(pos));
    return w;
}


void CurveListItemWidget::setVarEnabled(bool enabled){
    enableBox->setChecked(enabled);
}

void CurveListItemWidget::setVarVisible(bool visible){
    if(visible){
        visibleBox->setIcon(visibleIcon);
    }else{
        visibleBox->setIcon(hiddenIcon);
    }
    //visibleBox->setChecked(visible);
}


void CurveListItemWidget::setText(const QString lab){
    nameLabel->setText(lab);
}


void CurveListItemWidget::setColor(QColor col){
   //enableBox->setAutoFillBackground(true);
color = col;
   //enableBox->setPalette(col);
   //enableBox->setBackgroundRole(QPalette::Background);
}

void CurveListItemWidget::paintEvent(QPaintEvent * event){

    QPainter painter(this);
    painter.setPen(color);
    painter.setBrush(QBrush(color));
    painter.drawRect(enableBox->rect().translated(enableBox->pos()));
    QWidget::paintEvent(event);
}

