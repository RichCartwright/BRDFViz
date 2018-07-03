#include "VTKWindow.hpp"
#include "VTKIncludes.hpp"

#include "scene.hpp"
#include "global_config.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "out.hpp"
#include "sampler.hpp"
#include "render_driver.hpp"
#include "utils.hpp"
#include "config.hpp"

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
VTKWindow::VTKWindow()
{
    this->setupUi(this);

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    qvtkWidget->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    
    this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

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

    if(!sceneDir.isNull())
    {
	//We want to remember this path for next time
	previousPath = sceneDir;	
    }

    std::string directory = "";
    std::shared_ptr<Config> cfg; 
    std::string cfg_ext;
    std::string output_file;
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
    
	    Scene scene;
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

	    Camera camera = cfg->GetCamera(0.0f);

	    cfg->PerformPostCheck();

	    std::string base_output_file = output_file;

	    output_file = base_output_file; 
	    Camera c = camera;

	    //QFuture<void> renderDriver = QtConcurrent::run(RenderDriver::RenderFrame, scene, cfg, c, output_file);
	    //renderDriver.waitForFinished();

	    RenderDriver::RenderFrame(scene, cfg, c, output_file);
    }
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



