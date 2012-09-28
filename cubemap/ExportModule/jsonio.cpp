/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "jsonio.h"
#include "qjson/parser.h"
#include "qjson/serializer.h"
#include <QFile>
#include <QTextStream>

#include <QVariantMap>

JSonIO::JSonIO(QObject *parent) :
    QObject(parent)
{
    reset();
}

void JSonIO::reset(){
    parserState = 0;

    setOutputFile("empty");
    setTopFace("empty");
    setBottomFace("empty");
    setLeftFace("empty");
    setRightFace("empty");
    setFrontFace("empty");
    setBackFace("empty");
    setFrameStart(0);
    setFrameStop(0);
    setOutputWidth(0);
    setAntialiasing(0);
    setAaPattern("grid");
    frames.clear();

}

// {"cubemap": 1, "fov": 180, "pitch" : 0, "yaw" : 0, "roll" : 0, "zoom" : 0},
bool JSonIO::getFrame(int index, QVariantMap * cubemap, QVariantMap * fov, QVariantMap * pitch, QVariantMap * yaw, QVariantMap * roll, QVariantMap * zoom){
    if(frames.size() == 0){
        return false;
    }
    index -= frameStart;
    if(index > frames.size() - 1 || index < 0){
        return false;
    }

    QVariantMap frame = frames[index].toMap();
    *cubemap = frame["cubemap"].toMap();
    *fov =  frame["fov"].toMap();
    *pitch = frame["pitch"].toMap();
    *yaw = frame["yaw"].toMap();
    *roll = frame["roll"].toMap();
    *zoom = frame["zoom"].toMap();
    return true;
}

int JSonIO::getNextKeyFrame(int from, QVariantMap * cubemap, QVariantMap * fov, QVariantMap * pitch, QVariantMap * yaw, QVariantMap * roll, QVariantMap * zoom){
    QVariantMap cm;// = new QVariantMap();
    QVariantMap fv;// = new QVariantMap();
    QVariantMap ph;// = new QVariantMap();
    QVariantMap yw;// = new QVariantMap();
    QVariantMap rl;// = new QVariantMap();
    QVariantMap zm;// = new QVariantMap();
    bool ok = getFrame(from++, &cm, &fv, &ph, &yw, &rl, &zm);
    while(ok && !cm["key"].toBool() && !fv["key"].toBool() && !ph["key"].toBool() && !yw["key"].toBool() && !rl["key"].toBool() && !zm["key"].toBool()){
        ok = getFrame(from++, &cm, &fv, &ph, &yw, &rl, &zm);
    }
    if(ok){
        getFrame(--from, cubemap, fov, pitch, yaw, roll, zoom);
        return from;
    }
    return -1;
}

void JSonIO::addFrame(QVariantMap cubemap, QVariantMap fov, QVariantMap pitch, QVariantMap yaw, QVariantMap roll, QVariantMap zoom){
    QVariantMap frame;
    frame["cubemap"] = cubemap;
    frame["fov"] = fov;
    frame["pitch"] = pitch;
    frame["yaw"] = yaw;
    frame["roll"] = roll;
    frame["zoom"] = zoom;

    frames.append(frame);
}

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
          {"cubemap": 1, "fov": 180, "pitch" : 0, "yaw" : 0, "roll" : 0, "zoom" : 0},
          {"cubemap": 1, "fov": 180, "pitch" : 1, "yaw" : 2, "roll" : 3, "zoom" : 4},
          {"cubemap": 1, "fov": 180, "pitch" : 2, "yaw" : 3, "roll" : 4, "zoom" : 1},
          {"cubemap": 1, "fov": 180, "pitch" : 3, "yaw" : 4, "roll" : 1, "zoom" : 2},
          {"cubemap": 1, "fov": 180, "pitch" : 4, "yaw" : 1, "roll" : 2, "zoom" : 3}
        ]
}

  */
void JSonIO::saveJSon(QString filePath){
    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        parserState = 1;
        return;
    }

    QVariantMap root;
    root.insert("outputFile", getOutputFile());
    root.insert("topFace", getTopFace());
    root.insert("bottomFace", getBottomFace());
    root.insert("leftFace", getLeftFace());
    root.insert("rightFace", getRightFace());
    root.insert("frontFace", getFrontFace());
    root.insert("backFace", getBackFace());
    root.insert("frameStart", getFrameStart());
    root.insert("frameStop", getFrameStop());
    root.insert("outputWidth", getOutputWidth());
    root.insert("antialiasing", getAntialiasing());
    root.insert("aaPattern", getAaPattern());
    root.insert("frames", frames);

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(root);
    QTextStream out(&jsonFile);
    out.setCodec("UTF-8");
    out << json;
    jsonFile.close();
    qDebug() << "Saved to " << filePath;
}

void JSonIO::loadJSon(QString filePath){
    parserState = 0;
    QJson::Parser parser;
    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        parserState = 1;
        return;
    }

    QTextStream in(&jsonFile);
    QString json = in.readAll();
    jsonFile.close();

    bool ok;
    QVariantMap result = parser.parse(json.toAscii(), &ok).toMap();
    if (!ok) {
        parserState = 2;
    }

    setOutputFile(result["outputFile"].toString());

    setTopFace(result["topFace"].toString());
    setBottomFace(result["bottomFace"].toString());
    setLeftFace(result["leftFace"].toString());
    setRightFace(result["rightFace"].toString());
    setFrontFace(result["frontFace"].toString());
    setBackFace(result["backFace"].toString());

    setFrameStart(result["frameStart"].toInt());
    setFrameStop(result["frameStop"].toInt());
    setOutputWidth(result["outputWidth"].toInt());
    setAntialiasing(result["antialiasing"].toInt());
    setAaPattern(result["aaPattern"].toString());

    frames = result["frames"].toList();
}
