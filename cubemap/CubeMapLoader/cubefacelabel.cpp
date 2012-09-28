/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>
#include "cubefacelabel.h"
#include "fileutil.h"
#include "cubemaploader.h"
#include <QFile>
#include <QDragEnterEvent>
#include <QUrl>
#include <QFileInfo>
#include <QApplication>
#include <QRegExp>
#include <QPainter>


CubeFaceLabel::CubeFaceLabel(CubeFace cubeFace, QWidget *parent) :
    QLabel(parent)
{
    empty = true;
    currentFrame = 0;

    QFileInfo fileInfo(":/loader/CubeMapDefault.png");
    fileDir = fileInfo.absoluteDir();
    imageFile = fileInfo.fileName();

    face = cubeFace;
    setFixedSize(200,200);
    setPixmap(QPixmap(completeFilePath()));
    setAcceptDrops(true);
}

void CubeFaceLabel::dragEnterEvent ( QDragEnterEvent * event ){
    if(event->mimeData()->hasUrls()){
        QList<QUrl> urlList =  event->mimeData()->urls();
        QFile file(urlList[0].toLocalFile());
        QString ext = QFileInfo(file).suffix();
        qDebug() << ext;
        if(QString::compare(ext,"jpg",Qt::CaseInsensitive) != 0
                && QString::compare(ext,"png",Qt::CaseInsensitive) != 0
                && QString::compare(ext,"tga",Qt::CaseInsensitive) != 0
                && QString::compare(ext,"tif",Qt::CaseInsensitive) != 0){
            event->ignore();
            return;
        }
        event->accept();
        return;
    }
    event->ignore();

}

void CubeFaceLabel::dropEvent ( QDropEvent * event ){
    QString lastFile = completeFilePath();
    if(setImageFile(event->mimeData()->urls()[0].toLocalFile())){
        CubeFaceLabel * swapLabel = dynamic_cast<CubeFaceLabel *>(event->source());
        if(swapLabel){
            swapLabel->setImageFile(lastFile);
        }
        event->accept();
    }else{
        event->ignore();
    }
}

void CubeFaceLabel::mousePressEvent(QMouseEvent *event)
 {
     if (event->button() == Qt::LeftButton)
         dragStartPosition = event->pos();
 }

void CubeFaceLabel::mouseMoveEvent(QMouseEvent *event)
 {
     if (!(event->buttons() & Qt::LeftButton))
         return;
     if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
         return;

     QDrag *drag = new QDrag(this);
     QMimeData *mimeData = new QMimeData;
     QList<QUrl> url;
     url << QUrl(completeFilePath());
     mimeData->setUrls(url);
     drag->setMimeData(mimeData);
     Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
     qDebug() << dropAction;
 }

bool CubeFaceLabel::setImageFile(QString filePath){
    //QPixmap pixmap;
    /*
    if(!image.load(filePath)){
        return false;
    }
*/
    QFileInfo fileInfo(filePath);
    fileDir = fileInfo.absoluteDir();
    imageFile = fileInfo.fileName();
    framesList.clear();



    if(filePath != ":/loader/CubeMapDefault.png"){
       framesList = getFramesForFile(filePath);
       qDebug() << "Found " << framesList.size() << " frames";
    }
    emit imageChanged(face);
    int cframe = currentFrame;
    currentFrame--;
    return loadFrame(cframe);
}

bool CubeFaceLabel::changeFrame(QString filePath){
   // QPixmap pixmap;

    if(!image.load(filePath)){
        filePath = ":/loader/CubeMapDefault.png";
        image.load(filePath);
    }

    if(filePath == ":/loader/CubeMapDefault.png"){
        empty = true;
        //setPixmap(pixmap);
        emit imageChanged(face);
        return true;
    }


    empty = false;
    //imageFile = new QFile(filePath);
    faceSize = image.size();
   // qDebug() << "Face size: " << faceSize;

    if(image.width() > image.height()){
        image = image.scaledToWidth(200);
    }else{
        image = image.scaledToHeight(200);
    }

    repaint();
    emit imageChanged(face);
    return true;
}

QString CubeFaceLabel::getFrameFullPath(int frameIndex){
    if(framesList.size() == 0){
        return completeFilePath();
    }
    frameIndex += framesList.keys().first();
    if(framesList.keys().contains(frameIndex)){
        return QString(fileDir.absolutePath() + QChar('/') + framesList[frameIndex]);
    }else{
        return completeFilePath();
    }
}

bool CubeFaceLabel::loadFrame(int frame){
    if(frame != currentFrame){
        currentFrame = frame;
        return changeFrame(getFrameFullPath(frame));
    }else{
        return true;
    }
}

void CubeFaceLabel::paintEvent(QPaintEvent * event){
    QLabel::paintEvent(event);
    if(empty)
        return;
    //paint some information on top of the image
    QPainter painter(this);
    painter.drawImage(0,0,image);
    painter.setBrush(QBrush(QColor(0,255,0,200)));
    painter.drawRect(-1,15,width()+1,20);

    painter.drawText(5,30,"Frame " + QString::number(currentFrame) + " (out of " + QString::number(framesList.size())+" )" );
    painter.drawRect(-1,height()-35,width()+1,20);
    painter.drawText(5,height()-20,"W:"+QString::number(faceSize.width())+" H:"+QString::number(faceSize.height()));

}
