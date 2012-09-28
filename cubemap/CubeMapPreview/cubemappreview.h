/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CUBEMAPPREVIEW_H
#define CUBEMAPPREVIEW_H

#include <QDebug>

#include "convertutil.h"
#include "cubemapdef.h"
#include "curvevariable.h"
#include <QLabel>

//class CurveVariable;

class CubeMapPreview : public QLabel
{
    Q_OBJECT
public:
    explicit CubeMapPreview(int size, QWidget *parent = 0);
    void calculateImage();

    float fieldOfViewDeg() { return toDegrees(fov); }
    float fieldOfViewRad() { return fov; }
    void setFieldOfViewRad(float fovRad, bool refreshAfter = true){ fov = fovRad; if(refreshAfter) refresh(); }
    void setFieldOfViewDeg(float fovDeg, bool refreshAfter = true){ setFieldOfViewRad(toRadians(fovDeg), refreshAfter); }


    float pitchDeg() { return toDegrees(pitch); }
    float pitchRad() { return pitch; }
    void setPitchRad(float pitchRad, bool refreshAfter = true){ pitch = pitchRad;  if(refreshAfter) refresh(); }
    void setPitchDeg(float pitchDeg, bool refreshAfter = true){ setPitchRad(toRadians(pitchDeg), refreshAfter); }


    float yawDeg() { return toDegrees(yaw); }
    float yawRad() { return yaw; }
    void setYawRad(float yawRad, bool refreshAfter = true){ yaw = yawRad;  if(refreshAfter) refresh(); }
    void setYawDeg(float yawDeg, bool refreshAfter = true){ setYawRad(toRadians(yawDeg),refreshAfter); }


    float rollDeg() { return toDegrees(roll); }
    float rollRad() { return roll; }
    void setRollRad(float rollRad, bool refreshAfter = true){ roll = rollRad;  if(refreshAfter) refresh(); }
    void setRollDeg(float rollDeg, bool refreshAfter = true){ setRollRad(toRadians(rollDeg), refreshAfter); }


    float zoomFactor() { return zoom;}
    void setZoomFactor(float z, bool refreshAfter = true) { zoom = z;  if(refreshAfter) refresh(); }

protected:
    float fov;
    float pitch;
    float yaw;
    float roll;
    float zoom;
    bool shiftDown;
    float currentVal;

    QImage * temp;

    QPointF lastPos;
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent ( QKeyEvent * event );
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent( QContextMenuEvent * contextMenuEvent );

    void setVariableValue(CurveVariable * var, float at, float val);

private:
    QList<QImage*> faces;

    QAction *resetFovAction;
    QAction *resetZoomAction;
    QAction *resetYawAction;
    QAction *resetPitchAction;
    QAction *resetRollAction;
    QAction *resetAllAction;

    QAction *keyFovAction;
    QAction *keyZoomAction;
    QAction *keyYawAction;
    QAction *keyPitchAction;
    QAction *keyRollAction;
    QAction *keyAllAction;

    CurveVariable * fovVar;
    CurveVariable * zoomVar;
    CurveVariable * yawVar;
    CurveVariable * pitchVar;
    CurveVariable * rollVar;


signals:
    void previewChanged();
    /*
    void zoomChanged(float);
    void yawChanged(float);
    void pitchChanged(float);
    void rollChanged(float);
    */

public slots:
    void refreshVariablesValues(float min, float max) { if((currentVal>min || min == -1) && (currentVal<max || max == -1)){ variablesValues(currentVal); } }
    void variablesValues(float at);
    void setFace(QImage * image, CubeFace face/*, bool refresh = false*/);

    void setFovVar(CurveVariable * var){ fovVar = var; /* QObject::connect(var,SIGNAL(changed()),this,SLOT(refreshVariablesValues())); */ }
    void setZoomVar(CurveVariable * var){ zoomVar = var; /* QObject::connect(var,SIGNAL(changed()),this,SLOT(refreshVariablesValues())); */}
    void setYawVar(CurveVariable * var){ yawVar = var; /* QObject::connect(var,SIGNAL(changed()),this,SLOT(refreshVariablesValues())); */}
    void setPitchVar(CurveVariable * var){ pitchVar = var; /* QObject::connect(var,SIGNAL(changed()),this,SLOT(refreshVariablesValues())); */}
    void setRollVar(CurveVariable * var){ rollVar = var; /* QObject::connect(var,SIGNAL(changed()),this,SLOT(refreshVariablesValues())); */}

    void resetFov();
    void resetZoom();
    void resetYaw();
    void resetPitch();
    void resetRoll();
    void resetAll();

    void keyFov() { setVariableValue(fovVar, currentVal, fieldOfViewDeg()); }
    void keyZoom() { setVariableValue(zoomVar, currentVal, zoom); }
    void keyYaw() { setVariableValue(yawVar, currentVal, yawDeg()); }
    void keyPitch() { setVariableValue(pitchVar, currentVal, pitchDeg()); }
    void keyRoll() { setVariableValue(rollVar, currentVal, rollDeg()); }
    void keyAll() { keyFov(); keyZoom(); keyYaw(); keyPitch(); keyRoll();}

    QString showDegrees() { return "fov: " + QString::number(fieldOfViewDeg()) + ", yaw: " + QString::number(yawDeg()) + ", pitch:" + QString::number(pitchDeg()) + " roll:" + QString::number(rollDeg()); }
    void refresh();
};

#endif // CUBEMAPPREVIEW_H
