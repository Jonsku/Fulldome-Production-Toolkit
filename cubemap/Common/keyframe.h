/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QHash>
#include <QObject>
#include "curvevariable.h"

class KeyFrames
{
protected:
    KeyFrames();
    QHash<QString, CurveVariable *> variables;
    static int created;
    static KeyFrames * instance;

public:
    static KeyFrames * getInstance();
    CurveVariable * getVariable(QString varName);
    void addVariable(CurveVariable * var);
    bool isKeyFrame(int at);
    void removeKeyFrame(int at);
};

#endif // KEYFRAME_H
