#include "VTKWindow.hpp"
#include "VTKIncludes.hpp"

VTKWindow::VTKWindow()
{
    this->setupUi(this);

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    qvtkWidget->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    
    this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

    connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

void VTKWindow::slotExit()
{
    qApp->exit();
}
