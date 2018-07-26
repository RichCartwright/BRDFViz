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
        GLuint vertexShader;
        GLuint fragShader;
        void printContextInformation();
        void ShaderCompileStatus();
}; 

#endif // OGLImageViewer_H
