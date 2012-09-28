/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class CurveEditor;
class TimeLine;
class CubeMapPreview;
class CubeMapLoader;
class CurveVariable;
class IOWidget;

/**
  * The main widget initializes all the modules of the application and handles project saving and loading from the <i>File</i> menu.
  */
class MainWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * Main widget constructor.
     * Creates and initialize the different modules of the interface and arrange them in a layout.
     * @param parent A pointer to the parent QWidget (The main window)
     */
    explicit MainWidget(QWidget *parent = 0);

protected:
    CubeMapPreview * preview; ///< A pointer to the real-time fisheye preview module. See CubeMapPreview
    CubeMapLoader * loader; ///< A pointer to the cube maps loader module. See CubeMapLoader
    CurveEditor * curveEditor; ///< A pointer to the interpolation curve editor module. See CurveEditor
    IOWidget * importExport;///< A pointer to the rendering settings module. See IOWidget
    TimeLine * timeLine;///< A pointer to the timeline module. See TimeLine

private:
    /**
     * Add or update the application settings.
     */
    void writeSettings();
    /**
     * Read and apply previous application settings.
     * The last open project file will be automatically opened.
     */
    void readSettings();

signals:

public slots:
    /**
     * Change the range of frames displayed by the Curve Editor graph.
     * @param frameNum An integer value of the number of frame to show.
     */
    void setEditorRange(int frameNum);
    /**
     * Save the current project to a JSON file.
     * The previous saved file will be overwritten. However, if it is the first time the project is saved,
     * @ref saveAs() will be called instead.
     */
    void save();
    /**
      * Opens a file dialog and saves the current project to a JSON file.
      */
    void saveAs();
    /**
      * Opens a file dialog for the user to select a JSON file and load the project.
      * At the moment, all unsaved changes will be lost.
      */
    void load();
};

#endif // MAINWIDGET_H
