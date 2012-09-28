/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
/**
 * The main window holds a central widget composed of all the modules of the application.
 * Its only other role is to save its postion and size between sessions.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
    * The main window constructor.
    * Initialize the other UI components, adds a file menu and load previously saved application settings.
    * @param parent A ponter to the parent QWidget or null.
    */
    MainWindow(QWidget *parent = 0);

    /**
    * Does not do anything particular.
    */
    ~MainWindow();

protected:
    /**
     * When the window is closed, current application settings will be automatically saved.
     * This method accepts the event passed to it.
     * @param event A pointer to the QCloseEvent that contains context information about the event.
     */
    void closeEvent(QCloseEvent *event);

    /**
     * Saves the size and position of the window in the application settings.
     */
    void writeSettings();

    /**
     * Reads the size and position of the window from the application settings.
     */
    void readSettings();

};

#endif // MAINWINDOW_H
