#ifndef OGLImageViewer_H
#define OGLImageViewer_H

#include <QOpenGLFunctions_3_1>
#include <QOpenGLWidget>

class OGLImageViewer :  public QOpenGLWidget, 
                        public QOpenGLFunctions_3_1
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
        GLuint VertexArrayID;
        GLuint vertexBuffer;
        GLuint programID;
        GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
}; 

#endif // OGLImageViewer_H
