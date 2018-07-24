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

        void initializeGL() Q_DECL_OVERRIDE;
        void paintGL() Q_DECL_OVERRIDE;
        void resizeGL(int w, int h) Q_DECL_OVERRIDE;

protected:

private:
        void printContextInformation();
}; 

#endif // OGLImageViewer_H
