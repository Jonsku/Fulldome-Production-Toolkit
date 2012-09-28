/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef FRAMESSCENE_H
#define FRAMESSCENE_H

#include <QLabel>

class FramesScene : public QLabel
{
    Q_OBJECT
public:
    explicit FramesScene(QWidget *parent = 0);
    float displayedFrame();

protected:
    int currentFrame;
    int startFrame;
    int endFrame;

     void paintEvent(QPaintEvent *event);
     void mouseMoveEvent ( QMouseEvent * event );
     void mousePressEvent ( QMouseEvent * event );
     void mouseReleaseEvent ( QMouseEvent * event );
     void resizeEvent ( QResizeEvent * event );

    int getFrameAt(float x);


signals:
    void rangeChanged(float start, float stop);
    void currentFrameChanged(int f);

public slots:
     void stepForward();
     void stepBack();
     void toStart();
     void toEnd();
     void setFrame(int f) { currentFrame = f; update();}
     void setCurrentFrame(int f);
     void setStartFrame(int f);
     void setFrameNumber(int);

};

#endif // FRAMESSCENE_H
