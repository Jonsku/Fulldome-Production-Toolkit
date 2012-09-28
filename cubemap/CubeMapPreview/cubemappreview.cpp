/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include "cubemappreview.h"
#include "curvevariable.h"
#include <QPixmap>
#include <QImage>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QMenu>
#include <cmath>

CubeMapPreview::CubeMapPreview(int size, QWidget *parent) :
    QLabel(parent)
{

    //frame number
    currentVal = 0;

    fov = toRadians(180);
    zoom = 0;
    pitch = 0;
    yaw = 0;
    roll = 0;

    shiftDown = false;

    faces << NULL << NULL << NULL << NULL << NULL << NULL;

    //setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    setFixedSize(size,size);

    temp = new QImage(size,size,QImage::Format_ARGB32);
    //setSize(size, size);
    setPixmap(QPixmap(size, size));
    setFocusPolicy(Qt::ClickFocus);

    /*
    QAction *resetFovAction;
    QAction *resetZoomAction;
    QAction *resetYawAction;
    QAction *resetPitchAction;
    QAction *resetRollAction;
      */

    //reset actions
    resetFovAction = new QAction(tr("&Field of View"), this);
    connect(resetFovAction, SIGNAL(triggered()), this, SLOT(resetFov()));

    resetZoomAction = new QAction(tr("&Zoom"), this);
    connect(resetZoomAction, SIGNAL(triggered()), this, SLOT(resetZoom()));

    resetYawAction = new QAction(tr("&Yaw"), this);
    connect(resetYawAction, SIGNAL(triggered()), this, SLOT(resetYaw()));

    resetPitchAction = new QAction(tr("&Pitch"), this);
    connect(resetPitchAction, SIGNAL(triggered()), this, SLOT(resetPitch()));

    resetRollAction = new QAction(tr("&Roll"), this);
    connect(resetRollAction, SIGNAL(triggered()), this, SLOT(resetRoll()));

    resetAllAction = new QAction(tr("&All"), this);
    connect(resetAllAction, SIGNAL(triggered()), this, SLOT(resetAll()));

    //key action
    keyFovAction = new QAction(tr("&Field of View"), this);
    connect(keyFovAction, SIGNAL(triggered()), this, SLOT(keyFov()));

    keyZoomAction = new QAction(tr("&Zoom"), this);
    connect(keyZoomAction, SIGNAL(triggered()), this, SLOT(keyZoom()));

    keyYawAction = new QAction(tr("&Yaw"), this);
    connect(keyYawAction, SIGNAL(triggered()), this, SLOT(keyYaw()));

    keyPitchAction = new QAction(tr("&Pitch"), this);
    connect(keyPitchAction, SIGNAL(triggered()), this, SLOT(keyPitch()));

    keyRollAction = new QAction(tr("&Roll"), this);
    connect(keyRollAction, SIGNAL(triggered()), this, SLOT(keyRoll()));

    keyAllAction = new QAction(tr("&All"), this);
    connect(keyAllAction, SIGNAL(triggered()), this, SLOT(keyAll()));

    refresh();

}

void CubeMapPreview::setFace(QImage * image, CubeFace face/*, bool r*/){
    faces[face] = image;
    //if(r)
        refresh();
}

void CubeMapPreview::keyPressEvent ( QKeyEvent * event ){
    switch(event->key()){
        case Qt::Key_Shift:
            shiftDown = true;
            break;
    }
}

void CubeMapPreview::keyReleaseEvent ( QKeyEvent * event ){
    switch(event->key()){
        case Qt::Key_Shift:
            shiftDown = false;
            break;
    }
}

void CubeMapPreview::mouseReleaseEvent ( QMouseEvent * event ){
    event->accept();
}

void CubeMapPreview::mousePressEvent ( QMouseEvent * event ){
    lastPos = event->posF();
}


void CubeMapPreview::mouseMoveEvent ( QMouseEvent * event ){
    float offsetY = (event->posF().y() - lastPos.y())/(size().height());
    float offsetX = (event->posF().x() - lastPos.x())/(size().width());

    if(shiftDown){
        setZoomFactor(zoom + offsetY*0.5);
        setRollRad(roll + offsetX*0.5);
    }else{
        setYawRad(yaw + offsetX*0.5, false);
        setPitchRad(pitch + offsetY*0.5);
    }
    emit previewChanged();
}

void CubeMapPreview::resetFov(){
    setFieldOfViewDeg(180);
}


void CubeMapPreview::resetZoom(){
    setZoomFactor(0);
}

void CubeMapPreview::resetYaw(){
    setYawRad(0);
}

void CubeMapPreview::resetPitch(){
    setPitchRad(0);
}

void CubeMapPreview::resetRoll(){
    setRollRad(0);
}

void CubeMapPreview::resetAll(){
    setFieldOfViewDeg(180, false);

    setZoomFactor(0, false);

    setYawRad(0, false);

    setPitchRad(0, false);

    setRollRad(0);
}

void CubeMapPreview::contextMenuEvent( QContextMenuEvent * event ){
    QMenu menu;
    QMenu * resetMenu = menu.addMenu(tr("&Reset"));

            resetMenu->addAction(resetFovAction);
            resetMenu->addAction(resetZoomAction);
            resetMenu->addAction(resetPitchAction);
            resetMenu->addAction(resetYawAction);
            resetMenu->addAction(resetRollAction);
            resetMenu->addSeparator();
            resetMenu->addAction(resetAllAction);

    QMenu * keyMenu = menu.addMenu(tr("&Key"));

            keyMenu->addAction(keyFovAction);
            keyMenu->addAction(keyZoomAction);
            keyMenu->addAction(keyPitchAction);
            keyMenu->addAction(keyYawAction);
            keyMenu->addAction(keyRollAction);
            keyMenu->addSeparator();
            keyMenu->addAction(keyAllAction);
    menu.exec(event->globalPos());
}

void CubeMapPreview::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.drawImage(0, 0, *temp);
    painter.setPen(QPen(Qt::white));
    QString pitchTxt;
    pitchTxt += QString(tr("Pitch ") + QChar(':')) + QString::number(toDegrees(pitch),'f',2) + "°";

    QString yawTxt;
    yawTxt += QString(tr("Yaw: ") + QChar(':')) + QString::number(toDegrees(yaw),'f',2) + "°";

    QString rollTxt;
    rollTxt += QString(tr("Roll: ") + QChar(':')) + QString::number(toDegrees(roll),'f',2) + "°";

    QString zoomTxt;
    zoomTxt += QString(tr("Zoom: ") + QChar(':')) + QString::number(zoom,'f',2);

    QString fovTxt;
    fovTxt += QString(tr("FOV: ") + QChar(':')) + QString::number(toDegrees(fov),'f',2) + "°";

    painter.drawText(QPoint(5,15), pitchTxt);
    painter.drawText(QPoint(5,30), yawTxt);
    painter.drawText(QPoint(5,45), rollTxt);
    painter.drawText(QPoint(5,60), zoomTxt);

    painter.drawText(QPoint(350,15), fovTxt);

}

void CubeMapPreview::calculateImage(){

    temp->fill(qRgba( 0, 0, 0, 255 ));
    for(int y=0; y<temp->height(); y++) {
        QRgb *row = (QRgb *)temp->scanLine(y);

        for(int x=0; x<temp->width(); x++) {
            float dx = x-temp->width()/2.0;
            float dy = -y+temp->height()/2.0;
            float d = sqrt(dx*dx+dy*dy);

            //constrain to produce a circular fisheye
            if(d > temp->width()/2.0){
                    continue;
            }

            float theta =  ((d*fov)/temp->width()); //theta
            float phi = atan2(-dy,-dx)+roll; //phi; add angle to change roll
            float sx_p = sin(theta) * cos(phi);
            float sy_p = sin(theta) * sin(phi);
            float sz_p = cos(theta+zoom);

            //
            //The projection vector is rotated by a rotation matrix
            //which is the result of the multiplication of 3D roation on X (pitch) and Y (yaw) axes

            float cosPitch = cos(pitch);
            float sinPitch = sin(pitch);
            float cosYaw = cos(yaw);
            float sinYaw = sin(yaw);

            float sx = sx_p*cosYaw+sy_p*sinPitch*sinYaw-sz_p*cosPitch*sinYaw;
            float sy = sy_p*cosPitch+sz_p*sinPitch;
            float sz = sx_p*sinYaw-sy_p*sinPitch*cosYaw+sz_p*cosPitch*cosYaw;

            //determine which side of the box to use
            float abs_x = fabs(sx);
            float abs_y = fabs(sy);
            float abs_z = fabs(sz);

            float xs = 0.0;
            float ys = 0.0;

            CubeFace side;
            if(abs_x > abs_y) {
                    if(abs_x > abs_z) {
                            side = sx > 0.0 ?  LeftFace : RightFace;
                    }
                    else {
                            side = sz > 0.0 ? FrontFace : BackFace;
                    }
            }
            else {
                    if(abs_y > abs_z) {
                            side = sy > 0.0 ? BottomFace : TopFace;
                    }
                    else {
                            side = sz > 0.0 ? FrontFace : BackFace;
                    }
            }

            if(faces[side] == NULL){
                //no pixel info
                row[x] = qRgba( 0, 0, 255, 255 );
                continue;
            }

            switch(side){
                case FrontFace:
                    xs = rc(-sx/sz);
                    ys = rc(sy/sz);
                    break;
                case BackFace:
                    xs = rc(-sx/sz);
                    ys = rc(-sy/sz);
                    break;
                case LeftFace:
                    xs = rc(sz/sx);
                    ys = rc(sy/sx);
                    break;
                case RightFace:
                    xs = rc(sz/sx);
                    ys = rc(-sy/sx);
                    break;
                case TopFace:
                    xs = rc(sx/sy);
                    ys = rc(-sz/sy);
                    break;
                case BottomFace:
                    xs = rc(-sx/sy);
                    ys = rc(-sz/sy);
                    break;
            }

            if(xs<0){
                    xs = 0;
            }else if(xs>=1){
                    xs = 0.999;
            }

            if(ys<0){
                    ys = 0;
            }else if(ys>=1){
                    ys = 0.999;
            }

            //get the pixel
            int lX = (int)(xs * faces[side]->width());
            int lY = (int)(ys * faces[side]->height());

            //assign the pixel
            row[x] = ((QRgb *)faces[side]->scanLine(lY))[lX];
        }
    }
}

void CubeMapPreview::refresh(){
    qDebug() << "R";
    calculateImage();
    repaint();
    //setPixmap(QPixmap::fromImage(*calculateImage()));
}

void CubeMapPreview::variablesValues(float at){
//    qDebug() << "Display frame "<< at;
    currentVal = at;
    if(fovVar != NULL && fovVar->isEnabled() )
        setFieldOfViewDeg(fovVar->valueAt(at), false);

    if(zoomVar != NULL && zoomVar->isEnabled())
        setZoomFactor(zoomVar->valueAt(at), false);

    if(pitchVar != NULL  && pitchVar->isEnabled())
        setPitchDeg(pitchVar->valueAt(at), false);

    if(yawVar != NULL && yawVar->isEnabled())
        setYawDeg(yawVar->valueAt(at), false);

    if(rollVar != NULL && rollVar->isEnabled())
        setRollDeg(rollVar->valueAt(at), false);

    refresh();
}

void CubeMapPreview::setVariableValue(CurveVariable * var, float at, float val){
    //disconnect so the other values don't get overwritten
    //disconnect(var,SIGNAL(changed()),this,SLOT(refreshVariablesValues()));
    var->setValueAt(at, val);
    //connect(var,SIGNAL(changed()),this,SLOT(refreshVariablesValues()));
}




