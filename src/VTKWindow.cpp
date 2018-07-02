#include "VTKWindow.hpp"
#include "VTKIncludes.hpp"

#include <QFileDialog>

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
    //
    QString sceneName = QFileDialog::getOpenFileName(	this, 
		    					tr("Open Scene..."), "", 
							tr("Scene JSON (*.json);;All Files (*)")); 

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



