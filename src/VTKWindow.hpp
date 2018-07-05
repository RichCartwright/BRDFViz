#ifndef VTKWindow_H
#define VTKWindow_H

#include "ui_VTKWindow.h"
#include "ui_AboutMe.h"

#include "camera.hpp"
#include "scene.hpp"
#include "config.hpp"
#include "VTKIncludes.hpp"

#include <QMainWindow>
#include <QDialog>

class VTKWindow : public QMainWindow, private Ui::VTKWindow 
{
    Q_OBJECT

    QThread *pathThread;
public:
    VTKWindow();
    Scene scene;
    Camera camera;
    std::shared_ptr<Config> cfg;
    std::string output_file;
    vtkSmartPointer<vtkOrientationMarkerWidget> widget;
    
protected:
    QString previousPath = NULL;

protected slots:
    virtual void slotExit();
    virtual void slotAbout();
    virtual void slotOpen();
    void HandleThreadError(QString err);
    void UpdateStatusBar(QString status);

private:
    void SetupXYZCompass();
};



// About me class
class AboutMeDialog : public QDialog, private Ui::AboutMe
{
    Q_OBJECT

public:
    AboutMeDialog(QMainWindow *parent = Q_NULLPTR);
};
#endif

