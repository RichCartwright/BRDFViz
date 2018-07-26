#include "OGLImageViewer.hpp"
#include <QDebug>
#include <QtGui>

// I don't like doing this, because recompiling for a shader is absurd. 
//  but dont want the headache at the moment. 
//  TODO - Do this properly
const char* vertexSource = R"glsl(
    #version 330 core

    in vec2 position;
    
    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
    }

)glsl";

const char* fragSource = R"glsl(
    #version 330 core
    
    in vec3 Colour;
    out vec4 outColour;
    void main()
    {   
        outColour = vec4(Colour, 1.0);
    }
)glsl";

OGLImageViewer::OGLImageViewer(QWidget *parent) : QOpenGLWidget (parent)
{
}

void OGLImageViewer::initializeGL()
{
    initializeOpenGLFunctions();
    printContextInformation(); 
    glClearColor(1.0, 0.0, 0.0, 1.0);
    
    float imageVert[] = { 
            -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right

            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
            -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left
            -0.5f,  0.5f, 1.0f, 0.0f, 0.0f  // Top-left
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(imageVert), imageVert, GL_STATIC_DRAW);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    
    fragShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(vertexShader), glCompileShader(fragShader);
    ShaderCompileStatus();

}

void OGLImageViewer::paintGL()
{
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

void OGLImageViewer::ShaderCompileStatus()
{
    GLint compileStatus;
    char statusBuff[512];
    QString status;

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus); 
    glGetShaderInfoLog(vertexShader, 512, NULL, statusBuff);
    qDebug() << qPrintable("\n---Vertex Shader Callback---");
    status = (statusBuff); 
    if(status.isEmpty())
    {
        qDebug() << "Vertex Shader compilation successful\n"; 
    }
    else
    {
        qDebug() << qPrintable(status); 
    }
    status.clear();
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compileStatus);
    glGetShaderInfoLog(fragShader, 512, NULL, statusBuff);
    qDebug() << qPrintable("---Frag Shader Callback---");
    status = (statusBuff);
    if(status.isEmpty())
    {
        qDebug() << "Fragment Shader compilation successful\n";
    }
    else
    {
        qDebug() << qPrintable(status);
    }
} 
