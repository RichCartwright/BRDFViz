#include "OGLImageViewer.hpp"
#include <QDebug>
#include <QtGui>
#include <string>
#include <fstream>
#include <sstream>

/*
static const GLfloat imageVert[] = { 
     -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right

     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
     -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left
     -0.5f,  0.5f, 1.0f, 0.0f, 0.0f  // Top-left
};
*/

static const GLfloat imageVert[] = 
{ 
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
};

OGLImageViewer::OGLImageViewer(QWidget *parent) : QOpenGLWidget (parent)
{
}

void OGLImageViewer::initializeGL()
{
    initializeOpenGLFunctions();
    printContextInformation(); 
    glClearColor(1.0, 0.0, 0.0, 1.0);
    
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // TODO - Sort this
    programID = LoadShaders( "../src/vert.v", "../src/frag.f");

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(   GL_ARRAY_BUFFER, 
                    sizeof(imageVert), 
                    imageVert, 
                    GL_STATIC_DRAW);

    GLuint elementArray;
    glGenBuffers(1, &elementArray);

    GLuint elements[] = 
    {
        0, 1, 2, 
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArray);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

}

void OGLImageViewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID); 

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
       0,                  
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );
    
    glDrawElements(GL_TRIANGLES, 6, GL_FLOAT, (void*)0); 
    glDisableVertexAttribArray(0);

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

GLuint OGLImageViewer::LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{

    /**** Taken from here - http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/ ****/

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

	if(VertexShaderStream.is_open())
    {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
    else
    {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open())
    {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
    {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
    {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
    {
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
} 
