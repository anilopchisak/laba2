#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
#include "Pipeline.h"

GLuint VBO; // a global variable for storing a pointer to the vertex buffer
GLuint IBO;
GLuint gWorldLocation;
float Scale = 0.0f;
GLint success; GLchar InfoLog[1024];

constexpr auto WINDOW_WIDTH = 1980;
constexpr auto WINDOW_HEIGHT = 1250;

static const char* vertex = R"(
	#version 330 core
	layout (location = 0) in vec3 Pos;
	out vec4 vertexColor;
	uniform mat4 gWorld;
	void main()
	{
		gl_Position = gWorld * vec4(4 * Pos.x, 4 * Pos.y, Pos.z, 1.0);
		vertexColor = vec4(clamp(Pos, 0.0, 1.0), 1.0);
	})";

static const char* fragment = R"(
	#version 330 core
	out vec4 FragColor;
	in vec4 vertexColor;
	void main()
	{
		FragColor = vertexColor;
	})";


void checkerror(GLuint program, GLint success, GLenum ShaderType)
{
	if (!success)
	{
		if (ShaderType == 0)
		{
			glGetProgramInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
			std::cerr << "Error compiling shader type " << static_cast<int>(ShaderType) << InfoLog << "\n";
		}
		else
		{
			glGetShaderInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
			std::cerr << "Error linking shader program " << InfoLog << "\n";
		}	
	}
}

void addshader(GLuint ShaderProgram, const char* ShaderText, GLenum ShaderType)
{
	GLuint shader = glCreateShader(ShaderType);

	const GLchar* ShaderSource[1];
	ShaderSource[0] = ShaderText;
	glShaderSource(shader, 1, ShaderSource, nullptr);
	glCompileShader(shader);

	// Checking for vertex shader compilation errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	checkerror(shader, success, ShaderType);

	glAttachShader(ShaderProgram, shader);
}

void createShaders(const char* ShaderText_v, const char* ShaderText_f)
{
	
	GLuint ShaderProgram = glCreateProgram();
 
	addshader(ShaderProgram, ShaderText_v, GL_VERTEX_SHADER);
	addshader(ShaderProgram, ShaderText_f, GL_FRAGMENT_SHADER);

	// Checking for shader binding errors
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	checkerror(ShaderProgram, success, 0);

	glUseProgram(ShaderProgram);

	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	assert(gWorldLocation != 0xFFFFFFFF);
}

void RenderSceneCB() //draw
{
	glClear(GL_COLOR_BUFFER_BIT); //clearing the frame buffer using the color specified above

	Scale += 0.1f;

	Pipeline p;

	p.Scale(0.1f, 0.1f, 0.1f);
	p.WorldPos(0.0f, 0.0f, 3.0f);
	p.Rotate(0, Scale, 0);

	glm::vec3 CameraPos(0.0f, 0.0f, -3.0f);
	glm::vec3 CameraTarget(0.0f, 0.0f, 2.0f);
	glm::vec3 CameraUp(0.0f, 1.0f, 0.0f);
	p.SetCamera(CameraPos, CameraTarget, CameraUp);
	p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (GLfloat*)p.GetTrans());


	// Rendering

	glEnableVertexAttribArray(0); // Enable or disable the shared array of vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer, prepare it for rendering
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // data inside the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);

	// indicates that the current window should be redrawn and during operation
	// of the main loop GLUT render function will be called
	glutPostRedisplay(); 

	glutSwapBuffers(); //swap the background buffer and the frame buffer
}

int main(int argc, char** argv)
{
		// window and workplace setup

	glutInit(&argc, argv); //initialize GLUT, pass parameters
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //setup of GLUT options
	glutInitWindowSize(1024, 768); //window parameters
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 01"); //create the window and give it a title
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //setting the color of the window

	glutDisplayFunc(RenderSceneCB); //GLUT interacts with window system
	glutIdleFunc(RenderSceneCB);

	//initialize GLEW and check for errors
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}


	glm::vec3 Vertices[4];
	Vertices[0] = glm::vec3(-1.0f, -1.0f, -1.0f);
	Vertices[1] = glm::vec3(0.0f, -1.0f, 5.0f);
	Vertices[2] = glm::vec3(1.0f, -1.0f, -1.0f);
	Vertices[3] = glm::vec3(0.0f, 1.0f, 1.0f);


		// buffers (vertices)
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);


	unsigned int Indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 2, 1 };

	// buffers (index array)
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);


	createShaders(vertex, fragment);

	glutMainLoop(); //transferring control to GLUT, it's waiting for events in the window

	return 0;
}