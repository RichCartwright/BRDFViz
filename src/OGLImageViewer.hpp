#ifndef OGLImageViewer_H
#define OGLImageViewer_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class OGLImageViewer :  public QOpenGLWidget, 
                        public QOpenGLFunctions
{   
    Q_OBJECT

public: 
        OGLImageViewer(QWidget *parent = 0);

        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);

protected:

private:
        void printContextInformation();
}; 

#endif // OGLImageViewer_H
