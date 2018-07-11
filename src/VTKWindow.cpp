#include "VTKWindow.hpp"
//#include "VTKIncludes.hpp"

//#include "scene.hpp"
#include "global_config.hpp"
#include "texture.hpp"
//#include "camera.hpp"
#include "out.hpp"
#include "sampler.hpp"
#include "render_driver.hpp"
#include "utils.hpp"

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFuture>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>

VTKWindow::VTKWindow()
{
    qRegisterMetaType<std::vector<double>>("std::vector<double>");

    this->setupUi(this);

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    qvtkWidget->SetRenderWindow(renderWindow);

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(0.0, 0.0, 0.2);
    this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

    polyData = vtkSmartPointer<vtkPolyData>::New();
    points = vtkSmartPointer<vtkPoints>::New();
    colours = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colours->SetNumberOfComponents(3);
    colours->SetName("Colours");

    // Now we have made the pointers to the polydata and points
    //  we can bind them. So they can be updated by the render loop
    
    polyData->SetPoints(points);
    polyData->GetPointData()->SetScalars(colours);

    vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->AddInputData(polyData);
    glyphFilter->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyphFilter->GetOutputPort());
    mapper->Update();

    vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(3);

    renderer->AddActor(actor);

    vtkSmartPointer<vtkAxesActor> axes = 
	    vtkSmartPointer<vtkAxesActor>::New();
    //This need to be a class member, else it will go out of scope!
    widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    widget->SetOutlineColor(0.9, 0.5, 0.1);
    widget->SetOrientationMarker(axes);
    // QVTK handles its own interaction - It can still be overriden with a style though
    widget->SetInteractor(this->qvtkWidget->GetRenderWindow()->GetInteractor());
    // Top right-ish
    widget->SetViewport(0.75, 0.75, 1, 1);
    widget->SetEnabled(1); 
    widget->InteractiveOn();


    connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
    connect(this->actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
    connect(this->actionOpenFile, SIGNAL(triggered()), this, SLOT(slotOpen())); 
}

void VTKWindow::slotOpen()
{
    // Scene opening
    QString sceneDir = QFileDialog::getOpenFileName(	this, 
		    					tr("Open Scene..."), previousPath, 
							tr("Scene JSON (*.json);;All Files (*)")); 
    pathThread = new QThread;
    if(!sceneDir.isNull())
    {
	//We want to remember this path for next time
	previousPath = sceneDir;	
    }

    std::string directory = "";
    std::string cfg_ext;
    if(!sceneDir.isEmpty())
    {
	    std::tie(std::ignore, cfg_ext) = Utils::GetFileExtension(sceneDir.toStdString());
	    try
	    {
		if(cfg_ext == "json")
		{
		    cfg = ConfigJSON::CreateFromFile(sceneDir.toStdString());
		}
		else
		{
		    QMessageBox sceneErrorMessage;
		    sceneErrorMessage.critical(0, "Error", "Only JSON scenes are supported!");
		    sceneErrorMessage.setFixedSize(500, 200);
		    UpdateStatusBar("Only JSON scenes are supported");
		}
	    }
	    catch(ConfigFileException& ex)
	    {
		std::stringstream ss;
		ss << "Failed to load config: " << ex.what();

		QString ExString = QString::fromStdString(ss.str());

		QMessageBox ConfigFileExceptionMessage;
		ConfigFileExceptionMessage.critical(0, "Error", ExString); 
		ConfigFileExceptionMessage.setFixedSize(500, 200);
	    }

	    if(directory != "") directory += "/";
	    output_file = directory + cfg->output_file;
    
	   // Scene scene;
	    try
	    {
            cfg->InstallMaterials(scene);
            cfg->InstallScene(scene);
            cfg->InstallLights(scene);
            cfg->InstallSky(scene);
            scene.MakeThinglassSet(cfg->thinglass);
	    }
	    catch(ConfigFileException& ex)
	    {
            std::stringstream ss;
            ss << "Failed to load config: " << ex.what();
            QString ExString = QString::fromStdString(ss.str());
            QMessageBox ConfigFileExceptionMessage;
            ConfigFileExceptionMessage.critical(0, "Error", ExString); 
            ConfigFileExceptionMessage.setFixedSize(500, 200);
	    }

	    scene.Commit();

	    camera = cfg->GetCamera(0.0f);

	    cfg->PerformPostCheck();
	    UpdateStatusBar("Config successfully loaded");

	    std::string base_output_file = output_file;

	    output_file = base_output_file; 
	    //Camera c = camera;

	    // Loads the scene data through the constructor - 
	    // Just to keep QT threads happy
	    RenderDriver *renderDriver = new RenderDriver(scene, cfg, camera, output_file);

	    renderDriver->moveToThread(pathThread);
	    connect(renderDriver, SIGNAL(error(QString)), this, SLOT(HandleThreadError(QString)));
	    connect(pathThread, SIGNAL(started()), renderDriver, SLOT(RenderFrame())); 
	    connect(renderDriver, SIGNAL(finished()), pathThread, SLOT(quit()));
	    connect(renderDriver, SIGNAL(finished()), renderDriver, SLOT(deleteLater()));
	    connect(pathThread, SIGNAL(finished()), pathThread, SLOT(deleteLater()));
	    connect(renderDriver, SIGNAL(statusBarUpdate(QString)), pathThread, SLOT(UpdateStatusBar(QString)));
	    // connect the path data recieve to the render driver - Its gonna have to come
	    //  from much further away though! 
	    connect(renderDriver, SIGNAL(ReturnPathData(std::vector<double>)),
		    this, SLOT(RecievePathData(std::vector<double>)));
	    UpdateStatusBar("Starting render thread");
	    pathThread->start();
    }
}

void VTKWindow::UpdatePointCloud(std::vector<double> pathData)
{

    static constexpr auto step = 6;

    for(std::vector<double>::iterator i = std::begin(pathData);
            /*Empty*/ ; 
            std::advance(i, step))
    {
        if(std::distance(i, pathData.end()) < step)
            break;

        // Current position of the iterator
        size_t position = i - pathData.begin();
        points->InsertNextPoint(pathData.at(position), 
                                pathData.at(position+1),
                                pathData.at(position+2));

        double col[3] = {   pathData.at(position+3)*255,
                            pathData.at(position+4)*255,
                            pathData.at(position+5)*255 };
        colours->InsertNextTuple(col); 
        points->Modified();
     }

    //polyData->SetPoints(points);
    //polyData->GetPointData()->SetScalars(colours);
    //polyData->Modified();
}

void VTKWindow::SetupXYZCompass()
{

}

void VTKWindow::RecievePathData(std::vector<double> pathData)
{
    // Finally pass this on to qVTK
    UpdatePointCloud(pathData);
}

void VTKWindow::HandleThreadError(QString err)
{
	QMessageBox sceneErrorMessage;
	sceneErrorMessage.critical(0, "Error", err);
	sceneErrorMessage.setFixedSize(500, 200);
}

void VTKWindow::UpdateStatusBar(QString status)
{
	Ui_VTKWindow::statusBar->showMessage(status);
}

void VTKWindow::slotExit()
{
    qApp->exit();
}

void VTKWindow::slotAbout()
{	
    AboutMeDialog aboutMe(this);
    aboutMe.setModal(true);
    aboutMe.exec();
    aboutMe.show();
}

// For the about me dialog
AboutMeDialog::AboutMeDialog(QMainWindow* parent) : QDialog(parent)
{
    this->setupUi(this);
}



