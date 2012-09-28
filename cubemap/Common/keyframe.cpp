/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include <QDebug>
#include "keyframe.h"
/*--------------
    KEY FRAMES
  --------------*/


//QHash<QString, CurveVariable *> KeyFrames::variables;
KeyFrames * KeyFrames::instance;
int KeyFrames::created = 0;

KeyFrames::KeyFrames()
{

}

KeyFrames * KeyFrames::getInstance(){
    if(created == 0){
        instance = new KeyFrames();
        created++;
    }
    return instance;
}

void KeyFrames::addVariable(CurveVariable * var){
    if(variables.contains(var->name()))
        return;
    variables[var->name()] = var;
}

CurveVariable * KeyFrames::getVariable(QString varName){
     if(variables.contains(varName)){
         return variables[varName];
     }
     return NULL;
}


bool KeyFrames::isKeyFrame(int at){
    for(int i=0;i < variables.size();i++){
        if(variables[variables.keys().at(i)]->isKeyAt(at)){
            return true;
        }
    }
    return false;
}

void KeyFrames::removeKeyFrame(int at){
    for(int i=0;i < variables.size();i++){
        variables[variables.keys().at(i)]->removeKeyAt(at);
    }
}
