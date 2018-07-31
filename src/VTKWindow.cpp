#include "VTKWindow.hpp"

#include "global_config.hpp"
#include "texture.hpp"
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
#include <QString>
#include <QGraphicsPixmapItem>

class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
    static MouseInteractorStyle* New();

    MouseInteractorStyle()
    {
        selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        selectedActor = vtkSmartPointer<vtkActor>::New();
    }

    virtual void OnMouseMove()
    {
        vtkInteractorStyleTrackballCamera::OnMouseMove();

        //vtkCamera* cam = this->GetDefaultRenderer()->GetActiveCamera();
        //cam->SetViewUp(0.0, 1.0, 0.0);
    }

    virtual void OnRightButtonDown()
    {
        int* pos = this->GetInteractor()->GetEventPosition();

        vtkSmartPointer<vtkCellPicker> picker =
            vtkSmartPointer<vtkCellPicker>::New();
        picker->SetTolerance(0.008);
        picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

        if(picker->GetPointId() != -1)
        {
            vtkIdType id = picker->GetPointId();
            double pointPos[3] = {0.0, 0.0, 0.0};

            std::stringstream ss;
            ss << "Point " << picker->GetPointId() << " selected."; 
	        WindowReference->UpdateStatusBar(QString::fromStdString(ss.str()));

            if(!PolyData)
            {
		        QMessageBox sceneErrorMessage;
		        sceneErrorMessage.critical(0, "Error", "Window Interactor contains a nullptr to the polydata!\n Has it been attatched on creation?");
		        sceneErrorMessage.setFixedSize(500, 200);
                return;
            }
            else
            {
                vtkSmartPointer<vtkPolyData> linePolyData =
                    vtkSmartPointer<vtkPolyData>::New();
                vtkSmartPointer<vtkPoints> linePoints = 
                    vtkSmartPointer<vtkPoints>::New();
                vtkSmartPointer<vtkCellArray> lines =
                    vtkSmartPointer<vtkCellArray>::New();

                WindowReference->TextInformation->clear();

                for(vtkIdType i = id - (id % 6), j = 0; i < id - (id % 6) + 6; i++, j++)
                {
                    if(linesActor)
                    {
                        this->GetDefaultRenderer()->RemoveActor(linesActor); 
                    }
                    
                    PolyData->GetPoint(i, pointPos);

                    std::stringstream ss;
                    ss << "Point " << i << 
                        "\n   x:" << pointPos[0] <<
                        "   y:" << pointPos[1] << 
                        "   z:" << pointPos[2] << std::endl; 
                    linePoints->InsertNextPoint(pointPos); 

                    WindowReference->TextInformation->append(QString::fromStdString(ss.str()));
                }

                for(vtkIdType i = 0; i < linePoints->GetNumberOfPoints() - 1; i++)
                {
                    vtkSmartPointer<vtkLine> line = 
                        vtkSmartPointer<vtkLine>::New();
                    line->GetPointIds()->SetId(0, i);
                    line->GetPointIds()->SetId(1, i+1);
                    lines->InsertNextCell(line);
                }

                linePolyData->SetPoints(linePoints);
                linePolyData->SetLines(lines);
                vtkSmartPointer<vtkPolyDataMapper> mapper =
                    vtkSmartPointer<vtkPolyDataMapper>::New();

                mapper->SetInputData(linePolyData);

                linesActor->SetMapper(mapper);
                linesActor->GetProperty()->SetLineWidth(2);

                this->GetDefaultRenderer()->AddActor(linesActor);
                this->GetDefaultRenderer()->Render();
            }

        }
        
        // Call the forward function
        vtkInteractorStyleTrackballCamera::OnRightButtonDown();
    }

    vtkSmartPointer<vtkDataSetMapper> selectedMapper;
    vtkSmartPointer<vtkActor> selectedActor;
    vtkSmartPointer<vtkPolyData> PolyData;
    VTKWindow* WindowReference;

    vtkSmartPointer<vtkActor> linesActor = 
        vtkSmartPointer<vtkActor>::New();
};

vtkStandardNewMacro(MouseInteractorStyle);

VTKWindow::VTKWindow()
{
    qRegisterMetaType<std::vector<double>>("std::vector<double>");

    this->setupUi(this);

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    qvtkWidget->SetRenderWindow(renderWindow);

    // We need to initialise the graphics pixmap item first
    graphicsScene = new QGraphicsScene(this);
    outputImage = new ImageDisplay(   512, 512,
                                    this->ImageViewer->width(), 
                                    this->ImageViewer->height() );

    graphicsScene->addItem(outputImage);

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(0.0, 0.0, 0.2);
    this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

    this->TextInformation->setReadOnly(true);
    this->TextInformation->setFontPointSize(8);

    polyData = vtkSmartPointer<vtkPolyData>::New();
    points = vtkSmartPointer<vtkPoints>::New();
    colours = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colours->SetNumberOfComponents(3);
    colours->SetName("Colours");

    // Now we have made the pointers to the polydata and points
    // we can plug them into the pipeline. So they can be updated by the render loop
    // No orginal points need to be added. An empty pointer is fine.
    polyData->SetPoints(points);
    polyData->GetPointData()->SetScalars(colours);

    // Glyph filter
    vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
        vtkSmartPointer<vtkVertexGlyphFilter>::New();
    glyphFilter->AddInputData(polyData);
    glyphFilter->Update();

    // Mapper creation
    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyphFilter->GetOutputPort());
    mapper->Update();

    vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(4);

    vtkSmartPointer<MouseInteractorStyle> interactorOverride = 
        vtkSmartPointer<MouseInteractorStyle>::New();
    interactorOverride->SetDefaultRenderer(renderer);

    // Sort the interactor, we have to refernece it then replace it
    QVTKInteractor* interactor = this->qvtkWidget->GetInteractor();
    interactorOverride->PolyData = polyData;
    interactorOverride->WindowReference = this;

    interactor->SetInteractorStyle(interactorOverride);
    renderer->AddActor(actor);

    SetupXYZCompass();

    // Handle the UI connections
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
            return;
	    }

	    if(directory != "") 
        {
            directory += "/";
        }

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
            return;
	    }

	    scene.Commit();

	    camera = cfg->GetCamera(0.0f);

	    cfg->PerformPostCheck();
	    UpdateStatusBar("Config successfully loaded");

        // Now the config is sorted, lets make the QImage
        if(graphicsScene)
        {
            this->ImageViewer->setScene(graphicsScene);
        }
        this->ImageViewer->show();
	    
        std::string base_output_file = output_file;
	    output_file = base_output_file; 

	    // Loads the scene data through the constructor - 
	    // Just to keep QT threads happy
	    RenderDriver *renderDriver = new RenderDriver(scene, cfg, camera, output_file);

	    renderDriver->moveToThread(pathThread);
	    connect(renderDriver, SIGNAL(error(QString)), this, SLOT(HandleThreadError(QString)));
	    connect(pathThread, SIGNAL(started()), renderDriver, SLOT(RenderFrame())); 
	    connect(renderDriver, SIGNAL(finished()), pathThread, SLOT(quit()));
	    connect(renderDriver, SIGNAL(finished()), renderDriver, SLOT(deleteLater()));
	    connect(pathThread, SIGNAL(finished()), pathThread, SLOT(deleteLater()));
	    connect(renderDriver, SIGNAL(statusBarUpdate(QString)), this, SLOT(UpdateStatusBar(QString)));
	    connect(renderDriver, SIGNAL(ReturnPathData(std::vector<double>)),
		    this, SLOT(RecievePathData(std::vector<double>)));
	    UpdateStatusBar("Starting render thread");
	    pathThread->start();
    }
}

void VTKWindow::UpdatePointCloud(std::vector<double> pathData)
{
    // I might update the path data to contain a vector of double vectors (or arrays) 
    //  later
    
    // Get the pixel position of the sent path
    // STILL NEEDED - TODO 
    int XY[2] = { (int)pathData.at(0), (int)pathData.at(1) };

    static constexpr auto step = 6;
    for(std::vector<double>::iterator i = std::begin(pathData) + 2;
            /*Empty*/ ; 
            std::advance(i, step))
    {
        // end() - 3 is used because of the RGB at the end
        //  we dont want to use that in here.
        if(std::distance(i, pathData.end() - 3) < step)
        {
            break;
        }

        // Current position of the iterator
        size_t position = i - pathData.begin();

        // Data input goes point then colour
        points->InsertNextPoint(pathData.at(position), 
                                pathData.at(position+1),
                                pathData.at(position+2));

        double col[3] = {   pathData.at(position+3)*255,
                            pathData.at(position+4)*255,
                            pathData.at(position+5)*255 };

        colours->InsertNextTuple(col); 
        // Call the update for the points
        points->Modified();
    }
  
    int vectorSize = pathData.size();
    double testCol[] = {    pathData.at(vectorSize - 3), 
                            pathData.at(vectorSize - 2), 
                            pathData.at(vectorSize - 1)    };
    UpdateFinalImage(XY, testCol);
    
}

void VTKWindow::UpdateFinalImage(int *PixelPosition, double *PixelColour)
{
   outputImage->UpdateImage(  PixelPosition[0], PixelPosition[1], PixelColour,
                            this->ImageViewer->width(), this->ImageViewer->height()); 
}

void VTKWindow::SetupXYZCompass()
{
    vtkSmartPointer<vtkAxesActor> axes = 
	    vtkSmartPointer<vtkAxesActor>::New();

    //This need to be a class member, else it will go out of scope!
    widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    widget->SetOutlineColor(0.9, 0.5, 0.1);
    widget->SetOrientationMarker(axes);
    
    // QVTK handles its own interaction - It can still be overriden with a style though
    widget->SetInteractor(this->qvtkWidget->GetRenderWindow()->GetInteractor());
    // Top right
    widget->SetViewport(0.75, 0.75, 1, 1);
    widget->SetEnabled(1); 
    widget->InteractiveOn();
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



