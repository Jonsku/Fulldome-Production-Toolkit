/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "cubemaploader.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

#include <QFileSystemModel>

#include <QEvent>
#include <QPixMap>
#include <QGridLayout>
#include <QResource>
#include <QDragEnterEvent>
#include <QRegExp>
#include <cmath>


CubeMapLoader::CubeMapLoader(QWidget *parent) :
    QWidget(parent)
{
    currentVal = 0;

    QLabel * folderPathLabel = new QLabel("&Source directory: ");
    folderPath = new QLineEdit(this);
    folderPath->installEventFilter(this);
    folderPathLabel->setBuddy(folderPath);

    QLabel * frameIndexLabel = new QLabel("Show frame: ");
    frameNumberBox = new QLineEdit("0");
    frameNumberBox->setValidator(new QIntValidator(0,1000000,this));
    frameNumberBox->installEventFilter(this);

    keyFrameButton = new QPushButton("Key",this);

    model = new QFileSystemModel(this);
    fileTreeView = new QTreeView(this);
    fileTreeView->setModel(model);
    fileTreeView->setRootIndex(model->index(QDir::homePath()));
    fileTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    fileTreeView->setDragEnabled(true);
    fileTreeView->setMinimumWidth(200);

    QStringList list;
    list << "*.jpg" << "*.png" << "*.tga" << "*.tif";
    model->setNameFilters(list);
    model->setNameFilterDisables(false);

    //signal - slot plumbing
    for(int i=LeftFace; i<=BackFace;i++){
        labels[i] = new CubeFaceLabel((CubeFace)i,this);
        connect(labels[i],SIGNAL(imageChanged(CubeFace)),this,SLOT(updatePreview(CubeFace)));
        connect(this,SIGNAL(frameChanged(int)),labels[i],SLOT(loadFrame(int)));
    }
    connect(frameNumberBox,SIGNAL(editingFinished()),this,SLOT(showFrameFromBox()));
    connect(keyFrameButton,SIGNAL(clicked()),this,SLOT(keyValue()));



    QGridLayout * gridLayout = new QGridLayout(this);
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(folderPathLabel,0,0);
    gridLayout->addWidget(folderPath,0,1,1,4);


    gridLayout->addWidget(labels[LeftFace],3,0);

    gridLayout->addWidget(labels[TopFace],1,1,2,1);
    gridLayout->addWidget(labels[FrontFace],3,1);
    gridLayout->addWidget(labels[BottomFace],4,1);


    gridLayout->addWidget(labels[RightFace],3,2);

    gridLayout->addWidget(labels[BackFace],3,3);

    gridLayout->addWidget(frameIndexLabel,1,2);
    gridLayout->addWidget(frameNumberBox,1,3);
    gridLayout->addWidget(keyFrameButton,2,2);

    gridLayout->addWidget(fileTreeView,1,4,-1,-1);

    gridLayout->addItem(new QSpacerItem(10,10),5,1,1,3);

    gridLayout->setRowStretch(0,0);
    gridLayout->setRowStretch(1,0);
    gridLayout->setRowStretch(2,0);
    gridLayout->setRowStretch(3,0);
    gridLayout->setRowStretch(4,0);
    gridLayout->setRowStretch(5,1);

    gridLayout->setColumnStretch(0,0);
    gridLayout->setColumnStretch(1,0);
    gridLayout->setColumnStretch(2,0);
    gridLayout->setColumnStretch(3,0);
    gridLayout->setColumnStretch(4,1);




    setLayout(gridLayout);

    sourceDir = QDir(QDir::homePath());
}

bool CubeMapLoader::eventFilter(QObject *obj, QEvent *event){
    if(obj == folderPath && event->type() == QEvent::MouseButtonRelease){
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        sourceDir.absolutePath(),
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if(!dir.isEmpty()){
            sourceDir.setPath(dir);
            folderPath->setText(dir);

            model->setRootPath(sourceDir.path());
            fileTreeView->setRootIndex(model->index(sourceDir.path()));
        }
        return true;
    }

    if(obj == frameNumberBox && event->type() == QEvent::KeyPress){
        if( ((QKeyEvent *)event)->key() == Qt::Key_Up){
            frameNumberBox->setText(QString::number(frameNumberBox->text().toInt() + 1));
            return true;
        }else if( ((QKeyEvent *)event)->key() == Qt::Key_Down ){
            frameNumberBox->setText(QString::number(frameNumberBox->text().toInt() - 1));
            return true;
        }
    }
    return false;
}

void CubeMapLoader::variablesValues(float at){
    currentVal = at;
    int frame = (int)round(frameVar->valueAt(at));
    showFrame(frame);
}

 void CubeMapLoader::keyValue(){
    frameVar->setValueAt(currentVal, frameNumberBox->text().toInt());
 }

void CubeMapLoader::updatePreview(CubeFace face){
    emit faceChanged(( ( labels[face]->isEmpty() ) ? NULL :labels[face]->getImage() ), face);
}

void CubeMapLoader::showFrameFromBox(){
    emit frameChanged(frameNumberBox->text().toInt());
}

