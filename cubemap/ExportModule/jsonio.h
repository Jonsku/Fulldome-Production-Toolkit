/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef JSONIO_H
#define JSONIO_H

#include <QObject>

#include <QList>
#include <QVariant>

/*

{
    "topFace" : "~/burp/faceZ/.png",
    "bottomFace" : "~/burp/faceZ-/.png",
    "leftFace" : "~/burp/faceY/.png",
    "rightFace" : "~/burp/faceY-/.png",
    "frontFace" : "~/burp/faceX/.png",
    "backFace" : "~/burp/faceX-/.png",
    "frameStart" : 1,
    "frameStop" : 499,
    "outputWidth" : 4094,
    "antialiasing" : 1,
    "aaPattern" : "grid",
    "frames": [
          {"cubemap": 1, "fov": 180, "pitch" : 0, "yaw" : 0, "roll" : 0, "zoom" : 0, "cubemapKey" : true},
          {"cubemap": 1, "fov": 180, "pitch" : 1, "yaw" : 2, "roll" : 3, "zoom" : 4},
          {"cubemap": 1, "fov": 180, "pitch" : 2, "yaw" : 3, "roll" : 4, "zoom" : 1},
          {"cubemap": 1, "fov": 180, "pitch" : 3, "yaw" : 4, "roll" : 1, "zoom" : 2},
          {"cubemap": 1, "fov": 180, "pitch" : 4, "yaw" : 1, "roll" : 2, "zoom" : 3}
        ]
}

  */


class JSonIO : public QObject
{
    Q_OBJECT
public:
    explicit JSonIO(QObject *parent = 0);

    void setOutputFile(QString f){ outputFile = f; }
    void setTopFace(QString f){ topFace = f; }
    void setBottomFace(QString f){ bottomFace = f; }
    void setLeftFace(QString f) { leftFace = f; }
    void setRightFace(QString f){ rightFace = f; }
    void setFrontFace(QString f){ frontFace = f; }
    void setBackFace(QString f){ backFace = f; }
    void setFrameStart(int i){ frameStart = i; }
    void setFrameStop(int i){ frameStop = i; }
    void setOutputWidth(int i){ outputWidth = i; }
    void setAntialiasing(int i){ antialiasing = i; }
    void setAaPattern(QString f) { aaPattern = f; }

    QString getOutputFile() const { return outputFile; }
    QString getTopFace() const { return topFace; }
    QString getBottomFace()  const { return bottomFace; }
    QString getLeftFace()  const { return leftFace; }
    QString getRightFace() const { return rightFace; }
    QString getFrontFace() const { return frontFace; }
    QString getBackFace() const { return backFace; }
    int getFrameStart() const { return frameStart; }
    int getFrameStop() const { return frameStop; }
    int getOutputWidth() const { return outputWidth; }
    int getAntialiasing() const { return antialiasing; }
    QString getAaPattern() const { return aaPattern; }

    int getParserState() const { return parserState; }
    int getFrameNumber() const { return frames.size(); }

protected:
    QString outputFile;
    QString topFace;
    QString bottomFace;
    QString leftFace;
    QString rightFace;
    QString frontFace;
    QString backFace;
    int frameStart;
    int frameStop;
    int outputWidth;
    int antialiasing;
    QString aaPattern;

    QVariantList frames;
    //0 : ok, 1 : Couldn't open file, 2 : Error parsing file
    int parserState;
signals:

public slots:
    void loadJSon(QString filePath);
    void saveJSon(QString filePath);
    bool getFrame(int index, QVariantMap * cubemap, QVariantMap * fov, QVariantMap * pitch, QVariantMap * yaw, QVariantMap * roll, QVariantMap * zoom);
    int getNextKeyFrame(int from, QVariantMap * cubemap, QVariantMap * fov, QVariantMap * pitch, QVariantMap * yaw, QVariantMap * roll, QVariantMap * zoom);
    void addFrame(QVariantMap cubemap, QVariantMap fov, QVariantMap pitch, QVariantMap yaw, QVariantMap roll, QVariantMap zoom);
    void reset();
};

#endif // JSONIO_H
