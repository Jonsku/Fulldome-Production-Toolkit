/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include "cubefacedata.h"

#include <QString>
#include <QFileInfo>

CubeFaceData::CubeFaceData(CubeFace face){
    this->face = face;
}

void CubeFaceData::setFile(QString filePath){
    QFileInfo fileInfo(filePath);
    fileDir = fileInfo.absoluteDir();
    fileMask = fileInfo.fileName();
    framesList = getFramesForFile(filePath);
}

QString CubeFaceData::getFrameFullPath(int frameIndex){
    if(framesList.size() == 0){
        return QString(fileDir.absolutePath() + QChar('/') + fileMask);
    }
    frameIndex += framesList.keys().first();
    if(framesList.keys().contains(frameIndex)){
        return QString(fileDir.absolutePath() + QChar('/') + framesList[frameIndex]);
    }else{
        return QString(fileDir.absolutePath() + QChar('/') + fileMask);
    }
}

bool CubeFaceData::loadFrame(int frameIndex){
    if(/* !framesList.keys().contains(frameIndex) || */!image.load(getFrameFullPath(frameIndex)) ){
        image = QImage();
        return false;
    }
    return true;
}

