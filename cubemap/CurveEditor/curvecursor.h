/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CURVECURSOR_H
#define CURVECURSOR_H

#include <QObject>
#include <QRectF>
#include <QPen>

class QGraphicsScene;
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;

class CurveCursor : public QObject
{
    Q_OBJECT
public:
    CurveCursor(QGraphicsScene * scene, QObject *parent = 0, QPen pen = QPen());


    void setScene(QGraphicsScene * scene);
    void showHorizontal(bool show =  true);
    void showCoord(bool show =  true);
    void setRect(QRectF rect) { viewPortRect = rect; setPos(lastX, lastY); }
    void setPos(float x, float y);
    QVector<qreal> scaleDashPattern(float s);


protected:
    QGraphicsSimpleTextItem * vertCoordText;
    QGraphicsSimpleTextItem * horCoordText;
    QGraphicsLineItem * vertLine;
    QGraphicsLineItem * horLine;
    QRectF viewPortRect;
    QPointF viewScale;
    bool showHor;
    QPointF inverseScale;
    QVector<qreal> dashPattern;
    float lastX;
    float lastY;

public slots:
    void show(bool s = true);
    void hide() { show(false); }
    void setViewScale(QPointF scale) { viewScale = scale; }
    void counterSceneScale(QPointF sceneScale);
    void setZValue(qreal zV);
};

#endif // CURVECURSOR_H
