/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>

#include "iowidget.h"
#include "jsonio.h"
#include "cubemaploader.h"
#include "timeline.h"
#include "keyframe.h"
#include "renderthread.h"

#include <QScrollBar>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QFileDialog>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QEvent>
#include <QSettings>

IOWidget::IOWidget(CubeMapLoader * cubemapLoader, TimeLine * timeLine, QWidget *parent) :
    QWidget(parent)
{
    this->cubemapLoader = cubemapLoader;
    this->timeLine = timeLine;
    jsonIO = new JSonIO(this);

    renderThread = new RenderThread(this);

    QLabel * jsonFilePathLabel = new QLabel("&JSON file: ");
    jsonFilePath = new QLineEdit(this);
    jsonFilePath->installEventFilter(this);
    jsonFilePathLabel->setBuddy(jsonFilePath);

    QLabel * outputFilePathLabel = new QLabel("Output &destination: ");
    outputFilePath = new QLineEdit(this);
    outputFilePath->installEventFilter(this);
    outputFilePathLabel->setBuddy(outputFilePath);

    QLabel * outputWidthLabel = new QLabel("&Output size: ");
    outputWidth = new QLineEdit(this);
    outputWidth->setText("4096");
    outputWidth->setValidator(new QIntValidator(0,1000000,this));
    outputWidthLabel->setBuddy(outputWidth);

    QLabel * frameNumberingLabel = new QLabel("&Frame numbering: ");
    QLabel * frameNumberingToLabel = new QLabel(" to ");
    frameNumberingStart = new QLineEdit(this);
    frameNumberingStart->setText("0");
    frameNumberingStart->setValidator(new QIntValidator(-1000000,1000000,this));
    frameNumberingLabel->setBuddy(frameNumberingStart);

    frameNumberingStop = new QLineEdit(this);
    frameNumberingStop->setText("0");
    frameNumberingStop->setValidator(new QIntValidator(-1000000,1000000,this));

    QLabel * antialiasingLevelLabel = new QLabel("&Antialiasing level: ");
    antialiasingLevel = new QLineEdit(this);
    antialiasingLevel->setText("0");
    antialiasingLevel->setValidator(new QIntValidator(0,10,this));
    antialiasingLevelLabel->setBuddy(antialiasingLevel);

    QLabel * antialiasingPatternLabel = new QLabel("Antialiasing &pattern: ");
    antialiasingPattern = new QComboBox(this);
    antialiasingPatternLabel->setBuddy(antialiasingPattern);

    /*
    patternTypes.push_back("grid");
    patternTypes.push_back("rgrid");
    patternTypes.push_back("poisson");
    */
    antialiasingPattern->addItem("Grid (fastest)",QVariant("grid"));
    antialiasingPattern->addItem("Rotated Grid (fast, better quality)",QVariant("rgrid"));
    antialiasingPattern->addItem("Poisson distribution (slowest, best quality)",QVariant("poisson"));

    QLabel * pathToRendererLabel = new QLabel("Path to cubemap2fisheye: ");
    pathToRenderer = new QLineEdit(this);
    pathToRenderer->installEventFilter(this);
    pathToRendererLabel->setBuddy(pathToRenderer);

    renderButton = new QPushButton("Render!",this);
    connect(renderButton,SIGNAL(clicked()),this,SLOT(render()));

    renderLog = new QTextEdit(this);
    renderLog->setReadOnly(true);

    QGridLayout * gridLayout = new QGridLayout();
    int row = 0;
    gridLayout->addWidget(jsonFilePathLabel,row,0);
    gridLayout->addWidget(jsonFilePath,row++,1,1,3);

    gridLayout->addWidget(outputFilePathLabel,row,0);
    gridLayout->addWidget(outputFilePath,row++,1,1,3);

    gridLayout->addWidget(outputWidthLabel,row,0);
    gridLayout->addWidget(outputWidth,row++,1,1,3);

    gridLayout->addWidget(frameNumberingLabel,row,0);
    gridLayout->addWidget(frameNumberingStart,row,1);
    gridLayout->addWidget(frameNumberingToLabel,row,2);
    gridLayout->addWidget(frameNumberingStop,row++,3);

    gridLayout->addWidget(antialiasingLevelLabel,row,0);
    gridLayout->addWidget(antialiasingLevel,row++,1,1,3);

    gridLayout->addWidget(antialiasingPatternLabel,row,0);
    gridLayout->addWidget(antialiasingPattern,row++,1,1,3);

    gridLayout->addWidget(pathToRendererLabel,row,0);
    gridLayout->addWidget(pathToRenderer,row++,1,1,3);

    gridLayout->addWidget(renderButton,row++,0);
    gridLayout->addWidget(renderLog, row,0,1,-1);
    setLayout(gridLayout);

    //TODO: Could be added to settings aswell
    lastDir = QDir(QDir::homePath());
    lastOutputDir = QDir(QDir::homePath());

    readSettings();
}

bool IOWidget::eventFilter(QObject *obj, QEvent *event){
    if(obj == jsonFilePath && event->type() == QEvent::MouseButtonDblClick){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open JSON file"),
                                                        lastDir.absolutePath(),
                                                        tr("JSON files (*.json)"));
        if(!fileName.isNull()){
            lastDir = QFileInfo(fileName).absoluteDir();
            jsonFilePath->setText(fileName);
        }
        return true;
    }
    if(obj == outputFilePath && event->type() == QEvent::MouseButtonDblClick){
        QString fileName = QFileDialog::getSaveFileName(this, tr("Choose output dir and format"),
                                                        lastOutputDir.absolutePath(),
                                                        tr("Images files (*.png, *.tga, *.tiff, *.jpg)"));
        if(!fileName.isNull()){
            lastOutputDir = QFileInfo(fileName).absoluteDir();
            outputFilePath->setText(fileName);
        }
        return true;
    }
    if( obj == pathToRenderer && event->type() == QEvent::MouseButtonDblClick){

        QString dir = (pathToRenderer->text() != "") ? QDir().absoluteFilePath(pathToRenderer->text()) : QDir::homePath();
        QString fileName = QFileDialog::getOpenFileName(this, tr("Find cubemap2fisheye"),
                                                        dir,
                                                        tr("Executables (*)"));
        if(!fileName.isNull()){
            //lastOutputDir = QFileInfo(fileName).absoluteDir();
            pathToRenderer->setText(fileName);
            QSettings settings("j-u-t-t-u.net", "Cubemap Animator");
            settings.beginGroup("RenderSettings");
            settings.setValue("pathToRenderer", fileName);
            settings.endGroup();
        }
        return true;
    }
    /*
    if(obj == frameNumberBox && event->type() == QEvent::KeyPress){
        if( ((QKeyEvent *)event)->key() == Qt::Key_Up){
            frameNumberBox->setText(QString::number(frameNumberBox->text().toInt() + 1));
            return true;
        }else if( ((QKeyEvent *)event)->key() == Qt::Key_Down ){
            frameNumberBox->setText(QString::number(frameNumberBox->text().toInt() - 1));
            return true;
        }
    }
    */
    return false;
}

bool IOWidget::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"),
                                                    lastOutputDir.absolutePath(),
                                                    tr("JSON file (*.json)"));
    if(!fileName.isNull()){
        lastDir = QFileInfo(fileName).absoluteDir();
        jsonFilePath->setText(fileName);
        return save();
    }
    return false;
}

bool IOWidget::save(){
    int frameStart = 0;
    int frameStop = timeLine->getFrameNumber();
    if(jsonFilePath->text() == ""){
        return saveAs();
    }
    jsonIO->reset();

    jsonIO->setOutputFile(outputFilePath->text());
    jsonIO->setOutputWidth(outputWidth->text().toInt());
    jsonIO->setAntialiasing(antialiasingLevel->text().toInt());
    jsonIO->setAaPattern(antialiasingPattern->itemData(antialiasingPattern->currentIndex()).toString());
    jsonIO->setFrameStart(frameNumberingStart->text().toInt());
    jsonIO->setFrameStop(frameNumberingStop->text().toInt());

    jsonIO->setTopFace(cubemapLoader->getTopFaceFile());
    jsonIO->setBottomFace(cubemapLoader->getBottomFaceFile());
    jsonIO->setLeftFace(cubemapLoader->getLeftFaceFile());
    jsonIO->setRightFace(cubemapLoader->getRightFaceFile());
    jsonIO->setFrontFace(cubemapLoader->getFrontFaceFile());
    jsonIO->setBackFace(cubemapLoader->getBackFaceFile());


    KeyFrames * keyFrames = KeyFrames::getInstance();
    CurveVariable * cubemap = keyFrames->getVariable("Cubemap");
    CurveVariable * fov = keyFrames->getVariable("Fov");
    CurveVariable * pitch = keyFrames->getVariable("Pitch");
    CurveVariable * yaw = keyFrames->getVariable("Yaw");
    CurveVariable * roll = keyFrames->getVariable("Roll");
    CurveVariable * zoom = keyFrames->getVariable("Zoom");

    for(float f = frameStart; f<=frameStop; f++){
        //(QVariantMap cubemap, QVariantMap fov, QVariantMap pitch, QVariantMap yaw, QVariantMap roll, QVariantMap zoom)
        jsonIO->addFrame(cubemap->getSerializedFrame(f), fov->getSerializedFrame(f), pitch->getSerializedFrame(f), yaw->getSerializedFrame(f), roll->getSerializedFrame(f), zoom->getSerializedFrame(f));
    }

    jsonIO->saveJSon(jsonFilePath->text());

    //save to history
    QSettings settings("j-u-t-t-u.net", "Cubemap Animator");
    settings.beginGroup("ProjectHistory");
    settings.setValue("lastProject",jsonFilePath->text());
    settings.endGroup();
    return true;
}

int IOWidget::load(QString fileName){
    if(fileName == ""){
        fileName = QFileDialog::getOpenFileName(this, tr("Open JSON file"),
                                                    lastDir.absolutePath(),
                                                    tr("JSON files (*.json)"));
    }
    if(fileName.isNull()){
        return -1;
    }

    jsonIO->reset();
    jsonIO->loadJSon(fileName);
    if(jsonIO->getParserState() > 0){
        return -1;
    }

    //load render settings
    outputFilePath->setText(jsonIO->getOutputFile());
    outputWidth->setText(QString::number(jsonIO->getOutputWidth()));
    antialiasingLevel->setText(QString::number(jsonIO->getAntialiasing()));
    if(jsonIO->getAaPattern() == "rgrid"){
        antialiasingPattern->setCurrentIndex(1);
    }else if(jsonIO->getAaPattern() == "poisson"){
        antialiasingPattern->setCurrentIndex(2);
    }else{
        antialiasingPattern->setCurrentIndex(0);
    }
    frameNumberingStart->setText(QString::number(jsonIO->getFrameStart()));
    frameNumberingStop->setText(QString::number(jsonIO->getFrameStop()));


    //load cubemap
    cubemapLoader->setFaceFile(LeftFace,jsonIO->getLeftFace());
    cubemapLoader->setFaceFile(RightFace,jsonIO->getRightFace());
    cubemapLoader->setFaceFile(TopFace,jsonIO->getTopFace());
    cubemapLoader->setFaceFile(BottomFace,jsonIO->getBottomFace());
    cubemapLoader->setFaceFile(FrontFace,jsonIO->getFrontFace());
    cubemapLoader->setFaceFile(BackFace,jsonIO->getBackFace());

    //load key frames
    KeyFrames * keyFrames = KeyFrames::getInstance();
    CurveVariable * cubemapVar = keyFrames->getVariable("Cubemap");
    CurveVariable * fovVar = keyFrames->getVariable("Fov");
    CurveVariable * pitchVar = keyFrames->getVariable("Pitch");
    CurveVariable * yawVar = keyFrames->getVariable("Yaw");
    CurveVariable * rollVar = keyFrames->getVariable("Roll");
    CurveVariable * zoomVar = keyFrames->getVariable("Zoom");

    //reset curves
    cubemapVar->detachCurve();
    fovVar->detachCurve();
    pitchVar->detachCurve();
    yawVar->detachCurve();
    rollVar->detachCurve();
    zoomVar->detachCurve();

    QVariantMap * cubemap = new QVariantMap();
    QVariantMap * fov = new QVariantMap();
    QVariantMap * pitch = new QVariantMap();
    QVariantMap * yaw = new QVariantMap();
    QVariantMap * roll = new QVariantMap();
    QVariantMap * zoom = new QVariantMap();
    int from = jsonIO->getFrameNumber()-1;
    int next = jsonIO->getNextKeyFrame(from, cubemap, fov, pitch, yaw, roll, zoom);

    //int next = from;
    do{
        if(next>=0 && from == next){
            qDebug() << "Loading: Key frame at " << next;
            cubemapVar->deserializeFrame(from, *cubemap);
            fovVar->deserializeFrame(from, *fov);
            pitchVar->deserializeFrame(from, *pitch);
            yawVar->deserializeFrame(from, *yaw);
            rollVar->deserializeFrame(from, *roll);
            zoomVar->deserializeFrame(from, *zoom);
        }
        from--;
        next = jsonIO->getNextKeyFrame(from, cubemap, fov, pitch, yaw, roll, zoom);
    }while(from >= 0);

    jsonFilePath->setText(fileName);

    //save to history
    QSettings settings("j-u-t-t-u.net", "Cubemap Animator");
    settings.beginGroup("ProjectHistory");
    settings.setValue("lastProject",fileName);
    settings.endGroup();

    return jsonIO->getFrameNumber()-1;
}

void IOWidget::render(){
    if(renderThread->isRunning()){
        renderThread->quit();
        renderButton->setText("Render!");
        return;
    }

    if(outputFilePath->text() == ""){
        qDebug() << "No output destination specified...";
        QPalette pal = outputFilePath->palette();
        pal.setColor(QPalette::Background,Qt::red);
        outputFilePath->setPalette(pal);
        return;
    }
    outputFilePath->setPalette(QPalette());

    //the parameters need to be saved in a file first!
    if(!save()){
        return;
    }

    QStringList arguments;
    arguments << "-j" << jsonFilePath->text();// << QString::number(fps) << "-b" << QString::number(br) << "-i" << outputFolder+"/%06d"+imgFormat << "-y" << outputFolder+QChar('/')+fileName+format;
    qDebug() << arguments;


    renderButton->setText("Stop Render");
    renderLog->clear();
    // ffmpeg->start("/opt/local/bin/ffmpeg -codecs");
    renderThread->setProcess(this, pathToRenderer->text(),arguments);
    renderThread->start();
}

void IOWidget::displayInLog(QString text){

    qDebug() << "message is " << text.length();
    //keep displaying the bottom of the text edit if the bottom is already showing
    QScrollBar *sb = renderLog->verticalScrollBar();
    bool stickToBottom = (sb->value() == sb->maximum());

    //display text
    renderLog->insertPlainText(text);

    if(stickToBottom)
        sb->setValue(sb->maximum());
}

//
void IOWidget::renderDone(bool success){
    renderThread->quit();
    renderButton->setText("Render!");
}

void IOWidget::readSettings(){
    QSettings settings("j-u-t-t-u.net", "Cubemap Animator");
    settings.beginGroup("RenderSettings");
    pathToRenderer->setText(settings.value("pathToRenderer", "/Users/Jonsku/Documents/QtDev/JuttuFulldome/cubemap2fisheye-build-desktop/cubemap2fisheye").toString());
    settings.endGroup();
}

