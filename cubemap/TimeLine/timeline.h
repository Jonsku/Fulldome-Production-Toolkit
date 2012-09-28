/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QIcon>

class QScrollBar;
class QPushButton;
class QLineEdit;
class QTimeLine;
class FramesScene;
class Ruler;


class TimeLine : public QWidget
{
    Q_OBJECT
public:
    explicit TimeLine(QWidget *parent = 0);
    int getFrameNumber() { return frameNumber; }
protected:
    Ruler * ruler;
    FramesScene * framesScene;
    QScrollBar * scrollBar;
    QTimeLine *timeLine;
    QPushButton *playPauseButton;

    QPushButton *stepForwardButton;
    QPushButton *stepBackButton;

    QPushButton *toEndButton;
    QPushButton *toStartButton;

    QLineEdit * frameNumberBox;
    QLineEdit * fpsNumberBox;

    QIcon playIcon;
    QIcon pauseIcon;
    QIcon forwardIcon;
    QIcon backIcon;
    QIcon toStartIcon;
    QIcon toEndIcon;

    int fps;
    int frameNumber;

signals:
    void frameNumberChanged(int frmNum);
    void frameChanged(float fr);

public slots:
    void setFpsFromBox();
    void setFps(int fs);
    void setFrameNumberFromBox();
    void setFrameNumber(int frmNum);
    void changeFrame(int fr) { emit frameChanged(fr); }
    void setCurrentFrame(int f);
    void updateScrollbar();
    void updateTimeline();
    void playPause();
    void stop();
};

#endif // TIMELINE_H
