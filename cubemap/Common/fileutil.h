/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
/** @file
 * A collection of inline functions to find frames of a same sequence based on files name.
 * <p>CubeAnimator accepts any file name in the format
 * "name.extension". Furthermore, it will look for sequence of files
 * named "whateverXwhatever.extension" where X is a numeral. So if you
 * drop a file called "name.png" on one of the face, then files like
 * "name0.png" or "name21.png" will be used in the sequence. Also if you
 * load a file called "name1.png", it will detect all files named
 * "nameX.png". If it is called "name2top.png", then it will detect all
 * files named "nameXtop.png".  In case you a have file whose name
 * contains two or more numerals, such as "name1_1.png", CubeAnimator
 * will detect files names "name1_X.png" as being part of a the same
 * sequence.</p>
 */

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QDebug>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QDir>

/**
  * Returns a sorted list of file names of all the images in the same sequence as filePath, filtered using mask.
  * @param filePath A QString representing the path to a file
  * @param mask A QString used as a mask to match files in filePath's directory (e.g. "top_*.png")
  * @return A sorted QMap that matches frame position in the sequence to image file names from filePath's directory
  */
inline QMap<int, QString> getFramesForMask(QString filePath, QString mask){
    QMap<int, QString> sortedList;

    QFileInfo fileInfo(filePath);
    QDir fileDir = fileInfo.absoluteDir();

    QStringList filters;
    filters << mask;
    fileDir.setNameFilters(filters);
    QStringList framesList = fileDir.entryList(QDir::NoFilter, QDir::Name);
    if(framesList.size() == 1){
        sortedList[0] = framesList[0];
    }
    //there is only one file of this kind
    if(framesList.size() <= 1){
        return sortedList;
    }

    //sort the file list according to the frame number
    QRegExp fileNameMaskRegExp("^(.*\\D+)?(\\d*)\\..+$");

    for(int i=0; i<framesList.size();i++){
        fileNameMaskRegExp.indexIn(framesList[i]);
        sortedList[fileNameMaskRegExp.cap(2).toInt()] = framesList[i];
    }

    return sortedList;
}

/**
  * Returns a mask (e.g. "top_*.png") to find images of the same sequence as fileName
  * @param fileName A file name (extension included)
  * @return A mask that can be used to find other images of the same sequence as fileName
  */
inline QString getMaskForFile(QString fileName){
    if(!fileName.contains("?")){
        QRegExp fileNameMaskRegExp("^(.*\\D+)?\\d*(\\..+)$");
        fileNameMaskRegExp.indexIn(fileName);
        return fileNameMaskRegExp.cap(1)+QChar('*')+fileNameMaskRegExp.cap(2);
    }else{
        return fileName.replace("?", QChar('*'));
    }
}

/**
  * Returns a sorted list of file names of all the images in the same sequence as filePath.
  * @param filePath A QString representing the path to a file
  * @return A sorted QMap that matches frame position in the sequence to image file names from filePath's directory
  */
inline QMap<int, QString> getFramesForFile(QString filePath){
     QFileInfo fileInfo(filePath);
     QString mask = getMaskForFile(fileInfo.fileName());
     return getFramesForMask(filePath, mask);
}


#endif // FILEUTIL_H
