/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef RULER_H
#define RULER_H

#include <QWidget>

class Ruler : public QWidget
{
    Q_OBJECT
public:
    explicit Ruler(QWidget *parent = 0);
    explicit Ruler(Qt::Orientation orientation, QWidget *parent = 0);
    void setOrientation(Qt::Orientation orientation ){ direction = orientation; }
    Qt::Orientation orientation() const { return direction; }

    void leftToRight() { lToR = true; }
    void rightToLeft() { lToR = false; }

    void topToBottom() { lToR = true; }
    void bottomToTop() { lToR = false; }

protected:
    void init();
    void paintEvent ( QPaintEvent * event );
    Qt::Orientation direction;
    bool lToR;
    float start;
    float stop;

    bool autoMark;
    float majorMark;
    float minorMark;
    float scrollbarMargin;

signals:

public slots:
    void setScrollbarMargin(float margin);
    void setStop(float s);
    void setStart(float s);
    void setAutoMark(bool on = true);
    void setMajorMark(float m);
    void setMinorMark(float m);
    void setRulerRange(float s, float e);
};

#endif // RULER_H
