/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
/*
./cubemap2fisheye -W 4096 -H 4096 -O ./render/frame1.png -0 ~/burp/faceY/.png -1 ~/burp/faceY-/.png  -2 ~/burp/faceZ/.png -3 ~/burp/faceZ-/.png  -4 ~/burp/faceX/.png -5 ~/burp/faceX-/.png --start 0 --stop 500
  */

#include <QtCore/QCoreApplication>
#include <string>



#include "cubemapdef.h"
#include "convertutil.h"

#include <fileutil.h>
#include <jsonio.h>
#include "cubefacedata.h"

#include "tclap/CmdLine.h"

#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#include <QVector>
#include <QTime>

#include <QColor>

using namespace std;

//QString getMaskForFile(QString );
//#include "SwitchArg.h"
//#include "MultiSwitchArg.h"
//#include "UnlabeledValueArg.h"

QDir outputDir;
QString outputFileMask;
int width, height;
float fov = 3.14159265;
float yaw = 0, pitch = 0, roll = 0, zoom = 0;
bool show = false;
bool batch = false;
int start = 0;
int stop = 0;
uint aa = 0;
uint ss = 0;

QVector<CubeFaceData *> cubeMap;

void antiAliasing(QImage * in, QImage * out, int type, int factor, float rot){

    //to accumulate the color channels
    int r = 0;
    int g = 0;
    int b = 0;

    int s = in->width();

    int halfFactor = factor/2;
    int sampleGrid_X[factor];
    int sampleGrid_Y[factor];

    //to step the angle in the rotated grid
    float rStep = (2.0*3.14159265)/(float)factor;

    if(type == 1){ //rotated grid
        for(int i = 0; i < factor; i++) {
            sampleGrid_X[i] = (int)floor(((float)factor/2.0)*cos(rot));
            sampleGrid_Y[i] = (int)floor(((float)factor/2.0)*sin(rot));
            rot += rStep;
        }
    }else if(type == 2){ //random poisson distribution
        srand(time(NULL));
        for(int i = 0; i < factor; i++) {

            bool ok = false;
            int x = 0;

            int count = 1;
            while(!ok || count<=10) {
                x = floor(rand() % factor - (float)factor/2.0);
                ok = true;
                for(int p=0;p<i;p++) {
                    if(abs(sampleGrid_X[p]-x)<1) {
                        ok = false;
                        break;
                    }
                }
                count++;
            }
            sampleGrid_X[i] = x;

            x = 0;
            ok = false;
            count = 1;
            while(!ok || count<=10) {
                x = floor(rand() % factor - float(factor)/2.0);
                ok = true;
                for(int p=0;p<i;p++) {
                    if(abs(sampleGrid_Y[p]-x)<1) {
                        ok = false;
                        break;
                    }
                }
                count++;
            }
            sampleGrid_Y[i] = x;
        }
    }else{ //grid

        for(int i = 0; i < factor;i++) {
            sampleGrid_X[i] = i-halfFactor;
            sampleGrid_Y[i] = i-halfFactor;
        }

    }

    //down-sampling
    QRgb * outputPtr;
    QRgb pix;
    for(int i=0;i<(s/factor);i++) {
        outputPtr = (QRgb *)out->scanLine(i);
        for(int j=0;j<(s/factor);j++) {

            r = 0;
            g = 0;
            b = 0;
            //sample using the pattern calculated above
            for(int k= 0; k < factor; k++) {
                for(int l= 0; l < factor; l++) {
                    int x = (j*factor+halfFactor);
                    int y = (i*factor+halfFactor);

                    pix = ( (QRgb *)in->scanLine(y+sampleGrid_Y[l]) )[(x + sampleGrid_X[k])];
                    r += qRed(pix);
                    g += qGreen(pix);
                    b += qBlue(pix);
                }
            }
            //save the average
            outputPtr[j] = qRgb(r/(factor*factor), g/(factor*factor), b/(factor*factor));
        }
    }
    //clean up
};

void calculateFisheye(QImage * out){
    //	int[] lookupTable = new int[w*h];
    //	int c = 0;
    //	float camX= 0;
    //	float camY= 0;
    CubeFaceData* side;
    for(int y=0;y<out->height();y++) {
        QRgb *row = (QRgb *)out->scanLine(y);
        //uchar* ptr = (uchar*) (out->imageData + y * out->widthStep);
        for(int x=0;x<out->width();x++) {
            float dx = x-out->width()/2.0;
            float dy = -y+out->height()/2.0;
            float d = sqrt(dx*dx+dy*dy);
            //std::cerr << d << std::endl;

            //constrain to produce a circular fisheye
            if(d>out->width()/2.0){
                continue;
            }

            float theta =  ((d*fov)/out->width()); //theta
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


            if(abs_x > abs_y) {
                if(abs_x > abs_z) {
                    side = cubeMap[sx > 0.0 ? LeftFace :  RightFace];
                }
                else {
                    side = cubeMap[sz > 0.0 ? FrontFace : BackFace];
                }
            }
            else {
                if(abs_y > abs_z) {
                    side = cubeMap[sy > 0.0 ? BottomFace : TopFace];
                }
                else {
                    side = cubeMap[sz > 0.0 ? FrontFace : BackFace];
                }
            }

            //no need to continue if the face is empty
            if(side->getData()->isNull()){
                continue;
            }



            //Convert to range [0;1]
            if(side->getFace() == FrontFace){
                xs = rc(-sx/sz);
                ys = rc(sy/sz);
            }else if(side->getFace() == BackFace){
                xs = rc(-sx/sz);
                ys = rc(-sy/sz);
            }else if(side->getFace() == LeftFace){
                xs = rc(sz/sx);
                ys = rc(sy/sx);
            }else if(side->getFace() == RightFace){
                xs = rc(sz/sx);
                ys = rc(-sy/sx);
            }else if(side->getFace() == TopFace){
                xs = rc(sx/sy);
                ys = rc(-sz/sy);
            }else if(side->getFace() == BottomFace){
                xs = rc(-sx/sy);
                ys = rc(-sz/sy);
            }

            //constrain
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
            int lX = (int)(xs * side->getData()->width());
            int lY = (int)(ys * side->getData()->height());

            //assign the pixel
            row[x] = ((QRgb *)side->getData()->scanLine(lY))[lX];
        }
    }
};

int main(int argc, char *argv[])
{
    for(int i = LeftFace; i <= BackFace; i++){
        cubeMap.insert(i,new CubeFaceData((CubeFace)i));
    }

    JSonIO * jsonIO = new JSonIO();
    bool argFileLoaded = false;

    int faceFilled = 0;
    bool test = false;
    int frameNum;

    vector<string> patternTypes;
    patternTypes.push_back("grid");
    patternTypes.push_back("rgrid");
    patternTypes.push_back("poisson");

    //parse the command line parameters
    try{
        TCLAP::CmdLine cmd("Turns a cubemap into a fisheye projection.", ' ', "0.9");



        TCLAP::SwitchArg testSwitch("t", "test", "Perform a test render of a few frames to get an estimated render time.\n The rendered frames will be deleted from disk afterward.",true);
        cmd.add(testSwitch);

        TCLAP::ValueArg<string> jsonFileArg("j","json","Read parameters from a json file.\nAll parameters specifed on the command line are overridden by the one found in the file.",false,"my_batch.json","string",cmd);

        TCLAP::ValueArg<string> outputFileArg("o","output","The name of the output file. \n When batch processing, a wildcard ? or a number in the file name can be used to specify where to put the frame index in the output filename, otherwise it will added just before the file's extension (e.g. frame.png => frame1.png, frame_?_test.png => frame_1_test.png, frame100.png => frame1.png).",false,"frame_?.png","string",cmd);
        //cmd.xorAdd(outputFileArg, testSwitch);

        TCLAP::ValueArg<float> zoomArg("z","zoom","The zoom amount (0 by default)",false,0,"float",cmd);

        TCLAP::ValueArg<float> rollArg("e","roll","The roll rotation in degrees (0° by default)",false,0,"float",cmd);

        TCLAP::ValueArg<float> yawArg("y","yaw","The yaw rotation in degrees (0° by default)",false,0,"float",cmd);

        TCLAP::ValueArg<float> pitchArg("p","pitch","The pitch rotation in degrees (0° by default)",false,0,"float",cmd);

        TCLAP::ValueArg<float> fovArg("f","fov","The field of view of the fisheye in degrees (180° by default)",false,180,"float",cmd);

        //TCLAP::ValueArg<int> heightArg("H","height","The height of the output fisheye",true,4096,"int",cmd);

        TCLAP::ValueArg<int> widthArg("w","width","The width of the output fisheye",false,4096,"int",cmd);



        TCLAP::ValuesConstraint<string> allowedVals( patternTypes );

        TCLAP::ValueArg<string> patternTypeArg("g","mspattern","The type of pattern to use in multisample anti-aliasing. This argument is only taken into account if a multisampling factor greater than 0 is provided.\n Values are: grid (simple grid), rgrid (a rotated grid pattern), poisson (random pattern with poisson distribution).",false,"grid",&allowedVals,cmd);

        TCLAP::ValueArg<int> antiAliasingArg("a","antialising","The multisampling factor used for multisample anti-aliasing. 0 (antialising disabled) is the default.\n This value is multiplied by 2 to get the size of the over sampled fisheye render.\n This makes rendering much longer, but produce nicer images. A value between 1 and 3 should be enough.",false,0,"int",cmd);

        vector< TCLAP::ValueArg<string> * > faceArgs;
        faceArgs.push_back(new TCLAP::ValueArg<std::string>("0","left","Path to the image file containing the left face of the cube map.\n If start and stop arguments are set, the wildcard ? or a number is allowed and will be replaced by the frame index. If no wildcard is found then it is assumed that the frame number is just before the file extension. \nIf omitted, or if the file can not be accessed, a solid color will be used to fill the missing pixels.",false,"","string", cmd));
        faceArgs.push_back(new TCLAP::ValueArg<std::string>("1","right","Path to the image file containing the right face of the cube map.\n If start and stop arguments are set, the wildcard ? or a number is allowed and will be replaced by the frame index. If no wildcard is found then it is assumed that the frame number is just before the file extension. \nIf omitted, or if the file can not be accessed, a solid color will be used to fill the missing pixels.",false,"","string", cmd));
        faceArgs.push_back(new TCLAP::ValueArg<std::string>("2","top","Path to the image file containing the top face of the cube map.\n If start and stop arguments are set, the wildcard ? or a number is allowed and will be replaced by the frame index. If no wildcard is found then it is assumed that the frame number is just before the file extension. \nIf omitted, or if the file can not be accessed, a solid color will be used to fill the missing pixels.",false,"","string", cmd));
        faceArgs.push_back(new TCLAP::ValueArg<std::string>("3","bottom","Path to the image file containing the bottom face of the cube map.\n If start and stop arguments are set, the wildcard ? or a number is allowed and will be replaced by the frame index. If no wildcard is found then it is assumed that the frame number is just before the file extension. \nIf omitted, or if the file can not be accessed, a solid color will be used to fill the missing pixels.",false,"","string", cmd));
        faceArgs.push_back(new TCLAP::ValueArg<std::string>("4","front","Path to the image file containing the front face of the cube map.\n If start and stop arguments are set, the wildcard ? or a number is allowed and will be replaced by the frame index. If no wildcard is found then it is assumed that the frame number is just before the file extension. \nIf omitted, or if the file can not be accessed, a solid color will be used to fill the missing pixels.",false,"","string", cmd));
        faceArgs.push_back(new TCLAP::ValueArg<std::string>("5","back","Path to the image file containing the back face of the cube map.\n If start and stop arguments are set, the wildcard ? or a number is allowed and will be replaced by the frame index. If no wildcard is found then it is assumed that the frame number is just before the file extension. \nIf omitted, or if the file can not be accessed, a solid color will be used to fill the missing pixels.",false,"","string", cmd));

        TCLAP::ValueArg<int> startFrameArg("","start","The number associated to the first frame to be processed.\n This is associated with the stop argument and the file names provided for each faces to batch process a set of cube maps.\n If the stop argument is lower than start, the frame index will count down instead of up.",false,0,"int", cmd);
        TCLAP::ValueArg<int> stopFrameArg("","stop","The number associated to the last frame to be processed.\n This is associated with the start argument and the file names provided for each faces to batch process a set of cube maps.\n If the stop argument is lower than start, the frame index will count down instead of up.",false,0,"int", cmd);

        cmd.parse(argc, argv);
        cerr << cmd.getMessage() << endl;


        if(jsonFileArg.isSet()){

            jsonIO->loadJSon(QString::fromStdString(jsonFileArg.getValue()));
            if(jsonIO->getParserState() == 0){
                argFileLoaded = true;
            }

        }
        //first load cube map and check that at least one face contains frame
        for(int i = LeftFace; i <= BackFace; i++){
            if(faceArgs.at(i)->isSet()){
                cubeMap[i]->setFile(QString::fromStdString(faceArgs[i]->getValue()));
            }
            if(argFileLoaded){
                switch(i){
                case LeftFace:
                    if(jsonIO->getLeftFace() != "empty"){
                        cubeMap[i]->setFile(jsonIO->getLeftFace());
                    }
                    break;
                case RightFace:
                    if(jsonIO->getRightFace() != "empty"){
                        cubeMap[i]->setFile(jsonIO->getRightFace());
                    }
                    break;
                case TopFace:
                    if(jsonIO->getTopFace() != "empty"){
                        cubeMap[i]->setFile(jsonIO->getTopFace());
                    }
                    break;
                case BottomFace:
                    if(jsonIO->getBottomFace() != "empty"){
                        cubeMap[i]->setFile(jsonIO->getBottomFace());
                    }
                    break;
                case FrontFace:
                    if(jsonIO->getFrontFace() != "empty"){
                        cubeMap[i]->setFile(jsonIO->getFrontFace());
                    }
                    break;
                case BackFace:
                    if(jsonIO->getBackFace() != "empty"){
                        cubeMap[i]->setFile(jsonIO->getBackFace());
                    }
                    break;
                }
            }
            if(cubeMap[i]->frameNumber()>0){
                faceFilled++;
            }
            cerr << "Face " << i << " has " << cubeMap[i]->frameNumber() << " frames." << endl;
        }


    if(faceFilled == 0){
        cerr << "ERROR: None of the path specified point to an existing file. Aborting." << endl;
        exit(1);
    }

    //get the values of the parameters
    test = testSwitch.isSet();
    QFileInfo outputFileInfo;
    bool outputFileInfoSet = false;
    if(outputFileArg.isSet()){
       outputFileInfo = QFileInfo(QString::fromStdString(outputFileArg.getValue()));
       outputFileInfoSet = true;
    }

    if(argFileLoaded && jsonIO->getOutputFile() != "empty" && jsonIO->getOutputFile() != ""){
       outputFileInfo = QFileInfo(jsonIO->getOutputFile());
       outputFileInfoSet = true;
    }

    if(!outputFileInfoSet){
        cerr << "ERROR: You did not specify an output destination!. Aborting." << endl;
        exit(1);
    }

    outputDir = outputFileInfo.absoluteDir();
    if(!outputDir.exists()){
        if(!outputDir.mkpath(outputDir.absolutePath()) ){
            cerr << "ERROR: Couldn't create output directory " << outputDir.absolutePath().toStdString() << ". Aborting." << endl;
            exit(1);
        }
    }
    outputFileMask = getMaskForFile(outputFileInfo.fileName());

    width = widthArg.getValue();
    //height = heightArg.getValue();
    if(argFileLoaded && jsonIO->getOutputWidth() > 0){
        width = jsonIO->getOutputWidth();

    }
    height = width;

    if(startFrameArg.isSet()){
        start = startFrameArg.getValue();
    }

    if(argFileLoaded){
        start = jsonIO->getFrameStart();
    }

    if(stopFrameArg.isSet()){
        stop = stopFrameArg.getValue();
    }

    if(argFileLoaded){
        stop= jsonIO->getFrameStop();
    }

    if(!argFileLoaded && ((startFrameArg.isSet() && !stopFrameArg.isSet()) || (!startFrameArg.isSet() && stopFrameArg.isSet()))){
        if(start != 0){
            stop = start;
        }else{
            start = stop;
        }
        cerr << "WARNING: You only specified the start or the stop frame for batch rendering. You need to specify both. An attempt to render a single frame will be made." << endl;
    }

    batch = (start != stop);
    frameNum = abs(start-stop) + 1;

    //	float fov, float pitch, float yaw, float roll, float zoom
    if(fovArg.isSet()){
        //needs to be converted to radians
        fov =  toRadians(fovArg.getValue());
    }
    if(pitchArg.isSet()){
        //needs to be converted to radians
        pitch =  toRadians(pitchArg.getValue());
    }
    if(yawArg.isSet()){
        //needs to be converted to radians
        yaw =  toRadians(yawArg.getValue());
    }
    if(rollArg.isSet()){
        //needs to be converted to radians
        roll =  toRadians(rollArg.getValue());
    }
    if(zoomArg.isSet()){
        zoom =  zoomArg.getValue();
    }

    if(antiAliasingArg.isSet()){
        aa = antiAliasingArg.getValue();
    }

    if(argFileLoaded){
        aa = jsonIO->getAntialiasing();
    }

    string pattern;
    if(aa > 0){
        if(patternTypeArg.isSet()){
            pattern = patternTypeArg.getValue();
        }
        if(argFileLoaded){
            pattern = jsonIO->getAaPattern().toStdString();
        }
        ss = 0;
        for(uint i=0;i<patternTypes.size();i++){
            if( pattern == patternTypes[i]){
                ss = i;
                break;
            }
        }
        /*
        if(aa == patternTypes.size()){
            ss = 0;
        }
        */
    }
}catch (TCLAP::ArgException &e){
    cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    return 0;
}
QCoreApplication a(argc, argv);
if(test){
    cerr << "********* TEST RENDER *********" << endl;
    //reduce the range to max 3 frames
    //TODO: could be improved by picking random frames instead of 3 firsts.
    if(frameNum>3){
        stop -=  frameNum-3 * (start<stop ? 1 : -1);
    }
}

cerr << "Ouput width: " << width << "\tOuput height: " << height << "\tAntialiasing (0 = OFF): " << aa << "\tSampling pattern: " << patternTypes[ss] << endl;

QImage * superSample = NULL;
aa *= 2;
if(aa > 0){
    superSample = new QImage(width*aa, height*aa, QImage::Format_ARGB32);
}

QImage * out = new QImage(width, height, QImage::Format_ARGB32);

QString outFile;

QTime stopWatch = QTime();
long totalTime = 0;
int frameTime = 0;
int leftTime = 0;

qint64 totalSize = 0;
qint64 frameSize = 0;
/*
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int mseconds = 0;
    */
stopWatch.start();

//frame data from the json file
//getFrame(int index, QVariantMap * cubemap, QVariantMap * fov, QVariantMap * pitch, QVariantMap * yaw, QVariantMap * roll, QVariantMap * zoom);
QVariantMap cubemapFrame;
QVariantMap fovFrame;
QVariantMap pitchFrame;
QVariantMap yawFrame;
QVariantMap rollFrame;
QVariantMap zoomFrame;

int cubemapFrameIndex;
for(int i=start; (start<stop ? i<=stop : i>=stop); i += (start<stop ? 1 : -1)){

    outFile = outputFileMask;
    if(test){
        outFile = "t1e2s3t_"+outFile;
    }
    outFile = outputDir.absolutePath() + "/" + outFile.replace(QChar('*'), QString::number(i));
    cerr << "[ Frame " << i << " ]" << endl;

    cubemapFrameIndex = i;
    if(argFileLoaded && jsonIO->getFrame(i-start, &cubemapFrame, &fovFrame, &pitchFrame, &yawFrame, &rollFrame, &zoomFrame)){
        fov = toRadians(fovFrame["value"].toFloat());
        pitch = toRadians(pitchFrame["value"].toFloat());
        yaw = toRadians(yawFrame["value"].toFloat());
        roll = toRadians(rollFrame["value"].toFloat());
        zoom = zoomFrame["value"].toFloat();
        cubemapFrameIndex = cubemapFrame["value"].toFloat();
    }
    cerr << "Fov: " << fov << "\tPitch: " << pitch << "\tYaw: " << yaw << "\tRoll: " << roll << "\tZoom: " << zoom << endl;
    faceFilled = 0;

    for(int c = LeftFace; c <= BackFace; c++){
        cerr << "Face " << c << ": " << cubeMap[c]->getFrameFullPath(cubemapFrameIndex).toStdString() << endl;
        if(cubeMap[c]->loadFrame(cubemapFrameIndex))
            faceFilled++;
    }

    if(faceFilled == 0){
        cerr << "Nothing to render for this frame." << endl;
        continue;
    }
    if(aa > 0){
        calculateFisheye(superSample);
        antiAliasing(superSample, out, ss, aa, toRadians(20));
    }else{
        calculateFisheye(out);
    }

    out->save(outFile);
    frameSize = QFileInfo(outFile).size();
    totalSize += frameSize;

    if(test){
        //Don't count the time spent deleting the file
        frameTime = stopWatch.elapsed();
        QFile(outFile).remove();
        stopWatch.restart();
    }else{
        frameTime = stopWatch.restart();
    }

    totalTime += frameTime;
    cerr << "Frame rendered in " << msToHMSMS(frameTime).toStdString() << "." << endl;

    if(i!=stop){
        leftTime = ((totalTime/(abs(i-start)+1)) * abs(stop-i));
        cerr << "Estimated time left: " << msToHMSMS(leftTime).toStdString() << "." << endl;
    }


    cerr << "File size: " << bytesToKBMB(frameSize).toStdString() << "." << endl;
    if(!test && i != stop){
        cerr << "Estimated total size on disk: " << bytesToKBMB((totalSize/(abs(i-start)+1)) * abs(stop-i)).toStdString() << "." << endl;
    }
}

if(test){
    cerr << "Estimated render time for " << frameNum << " frame(s): " << msToHMSMS( (totalTime/(abs(stop-start)+1)) * frameNum).toStdString() << "." << endl;
    cerr << "Estimated total size on disk: " << bytesToKBMB( (totalSize/(abs(stop-start)+1)) * frameNum ).toStdString() << "." << endl;
}else{
cerr << "Done! Job took : " << msToHMSMS(totalTime).toStdString() << "." << endl;
cerr << "Total size on disk: " << bytesToKBMB(totalSize).toStdString() << "." << endl;
}
a.quit();
exit(0);
return a.exec();
};


