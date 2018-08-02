#ifndef VTKWindow_H
#define VTKWindow_H

#include "ui_VTKWindow.h"
#include "ui_AboutMe.h"

#include "camera.hpp"
#include "scene.hpp"
#include "config.hpp"
#include "VTKIncludes.hpp"
#include "ImageDisplay.hpp" 

#include <vector>
#include <QMainWindow>
#include <QDialog>

class VTKWindow : public QMainWindow, public Ui::VTKWindow 
{
    Q_OBJECT
    QThread *pathThread;
public:
    VTKWindow();
    Scene scene;
    Camera camera;
    std::shared_ptr<Config> cfg;
    std::string output_file;

public slots:
    void UpdateStatusBar(QString status);

protected:
    QString previousPath = "../scenes/";

protected slots:
    virtual void slotExit();
    virtual void slotAbout();
    virtual void slotOpen();
    void HandleThreadError(QString err);
    void RecievePathData(std::vector<double> pathData);

private:
    void SetupXYZCompass();
    void UpdatePointCloud(std::vector<double> pathData);
    void UpdateFinalImage(int *PixelPosition, double *PixelColour);

    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkPoints> points;
    vtkSmartPointer<vtkUnsignedCharArray> colours;
    vtkSmartPointer<vtkOrientationMarkerWidget> widget;
    std::vector<unsigned int> polyPathSize;

    ImageDisplay *outputImage = 0;
    QImage *image;
    QGraphicsScene *graphicsScene;
    QGraphicsPixmapItem *graphicPixmap;
    int multiSample;
};

class AboutMeDialog : public QDialog, private Ui::AboutMe
{
    Q_OBJECT

public:
    AboutMeDialog(QMainWindow *parent = Q_NULLPTR);
};
#endif

