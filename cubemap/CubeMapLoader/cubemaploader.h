/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef CubeMapLoader_H
#define CubeMapLoader_H

#include "cubefacelabel.h"
#include "cubemapdef.h"
#include "curvevariable.h"
#include <QWidget>
#include <QLineEdit>
#include <QFileSystemModel>
//#include <QListView>
#include <QTreeView>
#include <QDir>
#include <QPushButton>

class CubeMapLoader : public QWidget
{
    Q_OBJECT

public:
    explicit CubeMapLoader(QWidget *parent = 0);

    void setFrameVar(CurveVariable * var) { frameVar = var; }
    void setFaceFile(int face, QString filePath) { labels[face]->setImageFile(filePath); }
    QString getLeftFaceFile() { return labels[LeftFace]->completeFilePath(); }
    QString getRightFaceFile() { return labels[RightFace]->completeFilePath(); }
    QString getTopFaceFile() { return labels[TopFace]->completeFilePath(); }
    QString getBottomFaceFile() { return labels[BottomFace]->completeFilePath(); }
    QString getFrontFaceFile() { return labels[FrontFace]->completeFilePath(); }
    QString getBackFaceFile() { return labels[BackFace]->completeFilePath(); }

protected:
    QLineEdit * folderPath;
    QFileSystemModel *model;
    //QListView * fileListView;
    QTreeView * fileTreeView;
    QDir sourceDir;

    QLineEdit * frameNumberBox;
    QPushButton * keyFrameButton;

    CubeFaceLabel * labels[6];

    CurveVariable * frameVar;
    float currentVal;

    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void faceChanged(QImage * image, CubeFace face);
    void frameChanged(int i);

public slots:
    void keyValue();
    void variablesValues(float at);
    void updatePreview(CubeFace face);
    void showFrame(int i) { frameNumberBox->setText(QString::number(i)); emit frameChanged(i); }
    void showFrameFromBox();
};

#endif // CubeMapLoader_H
