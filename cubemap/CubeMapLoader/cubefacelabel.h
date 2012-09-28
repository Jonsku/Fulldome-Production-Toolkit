/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef LABEL_H
#define LABEL_H

#include "cubemapdef.h"
#include <QLabel>
#include <QMap>
#include <QDir>

class QPoint;
class QImage;

/** A holder (a QLabel) for a face of a cube map.
  * This UI element accepts a drag'n'drop of image files and shows a scaled down version of the image.</br>
  * Images can be swaped between two CubeFaceLabel.</br>
  * Information about the source file dimensions and the number of frames in the sequence are displayed on top of the image.
  */
class CubeFaceLabel : public QLabel
{
    Q_OBJECT
public:
    /**
      * Creates a new CubeFaceLabel assigned to a specific face of the cube map.
      * @param cubeFace Which face, identified as a CubeFace, corresponds to this label
      * @param parent A pointer to the parent QWidget of this label
      */
    explicit CubeFaceLabel(CubeFace cubeFace, QWidget *parent = 0);
    /**
      * @return Which face this label is showing, identified as a CubeFace
      */
    CubeFace cubeFace() const { return face; }
    /**
      * @return True if there is no image for this face, false otherwise.
      */
    bool isEmpty() const { return empty; }
    /**
      * @return The entire file path to the currently loaded image
      */
    QString completeFilePath() { return QString(fileDir.absolutePath() + QChar('/') + imageFile); }
    /**
      * @return A pointer to the QImage corresponding to the image of this face.
      */
    QImage * getImage() { return &image; }

protected:
    void paintEvent(QPaintEvent *);
    void dragEnterEvent ( QDragEnterEvent * event );
    void mousePressEvent(QMouseEvent *event);
    void dropEvent ( QDropEvent * event );
    void mouseMoveEvent(QMouseEvent *event);
    /**
      * Load a different image.
      * @param filePath The path to the new image
      * @return True if the image was loaded succesfully
      */
    bool changeFrame(QString filePath);
    /**
      * The entire file path to a specific frame in the sequence
      * @param frameIndex The index of the frame
      * @return The complete path to the frame
      */
    QString getFrameFullPath(int frameIndex);

 private:
    QMap<int, QString> framesList;
    QImage image;

    CubeFace face;
    QPoint dragStartPosition;
    QString imageFile;
    QDir fileDir;
    QSize faceSize;
    int currentFrame;

    bool empty;

signals:
    /**
      * Sent when a new image has been loaded for this face
      * @param Which face of the cube map changed
      */
    void imageChanged(CubeFace);

public slots:
    /**
      * Load a new image
      * @param filePath The path to the image file
      * @return True if the image was loaded succesfully, false otherwise
      */
    bool setImageFile(QString filePath);
    /**
      * Load a frame in the sequence
      * @param frame The index of the frame to load
      * @return True if the image was loaded succesfully, false otherwise
      */
    bool loadFrame(int frame);

};

#endif // LABEL_H
