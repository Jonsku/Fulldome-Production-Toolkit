/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CUBEFACE_H
#define CUBEFACE_H

#include "cubemapdef.h"
#include <fileutil.h>
#include <QMap>
#include <QDir>
#include <QImage>

class QString;


class CubeFaceData
{
public:
    CubeFaceData(CubeFace face);

    void setFile(QString filePath);

    CubeFace getFace() const { return face; }
    QImage * getData() { return &image; }

    int frameNumber() { return framesList.size(); }
    int firstFrameIndex() { return framesList.keys()[0]; }
    int lastFrameIndex() { return framesList.keys().last(); }
    bool isSingleFrame() { return framesList.empty(); }

    bool loadFrame(int frameIndex);
    QString getFrameFullPath(int frameIndex);


private:
     QMap<int, QString> framesList;
     CubeFace face;
     QImage image;
     QString fileMask;
     QDir fileDir;

};

#endif // CUBEFACE_H
