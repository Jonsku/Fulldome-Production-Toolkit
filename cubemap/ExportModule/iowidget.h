/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef IOWIDGET_H
#define IOWIDGET_H

#include <QWidget>
#include <QDir>
#include <QVariantMap>

class QLineEdit;
class QComboBox;
class QTextEdit;
class JSonIO;
class CurveVariable;
class CubeMapLoader;
class TimeLine;
class QPushButton;
class RenderThread;

class IOWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IOWidget(CubeMapLoader * cubemapLoader, TimeLine * timeLine, QWidget *parent = 0);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    CubeMapLoader * cubemapLoader;
    TimeLine * timeLine;
private:
    QLineEdit * jsonFilePath;
    QLineEdit * outputFilePath;
    QLineEdit * outputWidth;
    QLineEdit * frameNumberingStart;
    QLineEdit * frameNumberingStop;
    QLineEdit * antialiasingLevel;
    QComboBox * antialiasingPattern;
    QDir lastDir;
    QDir lastOutputDir;

    QLineEdit * pathToRenderer;
    QPushButton * renderButton;
    QTextEdit * renderLog;

    JSonIO * jsonIO;
    RenderThread * renderThread;

signals:

public slots:
    int load(QString file = "");
    bool saveAs();
    bool save();
    void render();
    void renderDone(bool success);
    void displayInLog(QString text);
    void readSettings();
};

#endif // IOWIDGET_H
