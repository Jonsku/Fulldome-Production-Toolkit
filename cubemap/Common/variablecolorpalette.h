/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef VARIABLECOLORPALETTE_H
#define VARIABLECOLORPALETTE_H

#include <QObject>
#include <QColor>

class VariableColorPalette : public QObject
{
    Q_OBJECT
public:
    explicit VariableColorPalette(QColor base, QObject *parent = 0);
    static QColor getNextColor();

protected:
   static QColor base;
   static int lastIndex;
signals:

public slots:

};

#endif // VARIABLECOLORPALETTE_H
