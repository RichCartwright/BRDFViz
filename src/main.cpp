//    VTK   
#include "VTKIncludes.hpp"
#include "VTKWindow.hpp"

int main(int argc, char** argv)
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
    VTKWindow qvtkWindow;
    qvtkWindow.show();

    return app->exec();
}
