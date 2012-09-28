/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#include "variablecolorpalette.h"

int randInt(int low, int high){
    return qrand() % ((high + 1) - low) + low;
}

int VariableColorPalette::lastIndex;
QColor VariableColorPalette::base;

VariableColorPalette::VariableColorPalette(QColor base, QObject *parent) :
    QObject(parent)
{
    qsrand(0);
    VariableColorPalette::base = base;
    VariableColorPalette::lastIndex = 0;
}

QColor VariableColorPalette::getNextColor(){
    int red = randInt(0,255);
    int green = randInt(0,255);
    int blue = randInt(0,255);

    red = (red + base.red()) / 2;
    green = (green + base.green()) / 2;
    blue = (blue + base.blue()) / 2;

    QColor col(red, green, blue);
    col.setHsl((col.hue()+(lastIndex++)*10)%255,col.saturation(), col.value());
    return col;
}
