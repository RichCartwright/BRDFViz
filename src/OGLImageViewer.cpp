#include "OGLImageViewer.hpp"
#include <QDebug>
#include <QtGui>
OGLImageViewer::OGLImageViewer(QWidget *parent) : QOpenGLWidget (parent)
{
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	setFormat(format);
}

void OGLImageViewer::initializeGL()
{
    //initializeOpenGLFunctions();
    printContextInformation(); 
    glClearColor(1.0, 0.0, 0.0, 1.0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void OGLImageViewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void OGLImageViewer::resizeGL(int w, int h)
{

}

void OGLImageViewer::printContextInformation()
{
    // Taken from here - 
    //  http://www.trentreed.net/blog/qt5-opengl-part-0-creating-a-window/

    QString glType;
    QString glVersion;
    QString glProfile;

    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL"; 
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
    switch(format().profile())
    {
        CASE(NoProfile);
        CASE(CoreProfile);
        CASE(CompatibilityProfile);
    }
#undef CASE

    qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << 
        qPrintable(glProfile) << ")"; 
} 
