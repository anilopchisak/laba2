#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector

GLuint VBO; // a global variable for storing a pointer to the vertex buffer
static float x = 0; static float y = 0; float scale_x = 0.001; float scale_y = 0.001;
GLuint gWorldLocation;

static const char* vertex = R"(
	#version 330 core
	layout (location = 0) in vec3 Pos;
	out vec4 vertexColor;
	uniform mat4 gWorld;
	void main()
	{
		gl_Position = gWorld * vec4(Pos.x, Pos.y, Pos.z, 1.0);
		vertexColor = vec4(0.0, 1.0, 1.0, 1.0);
	})";
static const char* fragment = R"(
	#version 330 core
	out vec4 FragColor;
	in vec4 vertexColor;
	void main()
	{
		FragColor = vertexColor;
	})";

void createShaders(const char* ShaderText_v, const char* ShaderText_f)
{
	GLint success; 	GLchar InfoLog[1024];
	GLuint ShaderProgram = glCreateProgram();


	// vertex shader 
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertexShaderSource[1];
	vertexShaderSource[0] = ShaderText_v;
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// ѕроверка на наличие ошибок компилировани€ вершинного шейдера
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", GL_VERTEX_SHADER, InfoLog);
	}


	// fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSource[1];
	fragmentShaderSource[0] = ShaderText_f;
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", GL_FRAGMENT_SHADER, InfoLog);
	}


	// —в€зывание шейдеров
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);
	// ѕроверка на наличие ошибок св€зывани€ шейдеров
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", InfoLog);
	}

	glUseProgram(ShaderProgram);

	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	assert(gWorldLocation != 0xFFFFFFFF);
}

void RenderSceneCB() //draw
{
	glm::mat4 WorldM;
	WorldM[0][0] = 1.0f; WorldM[0][1] = 0.0f; WorldM[0][2] = 0.0f; WorldM[0][3] = x;
	WorldM[1][0] = 0.0f; WorldM[1][1] = 1.0f; WorldM[1][2] = 0.0f; WorldM[1][3] = y;
	WorldM[2][0] = 0.0f; WorldM[2][1] = 0.0f; WorldM[2][2] = 1.0f; WorldM[2][3] = 0.0f;
	WorldM[3][0] = 0.0f; WorldM[3][1] = 0.0f; WorldM[3][2] = 0.0f; WorldM[3][3] = 1.0f;
	
	if (x >= 1 || x <= -1) scale_x *= -1; if (y >= 1 || y <= -1) scale_y *= -1;
	x += scale_x; y += scale_y;

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &WorldM[0][0]);


	// –ендеринг

	glClear(GL_COLOR_BUFFER_BIT); //clearing the frame buffer using the color specified above

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind (прив€зывать) the buffer, prepare it for rendering
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // data inside the buffer (index, 
	glEnableVertexAttribArray(0); // Enable or disable the shared (общий) array of vertex attributes

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	// показывает, что текущее окно должно быть отрисованно заново и в течении 
	// главного цикла GLUT функци€ рендера будет вызвана
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
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f); //setting the color of the window

	glutDisplayFunc(RenderSceneCB); //GLUT interacts with window system
	glutIdleFunc(RenderSceneCB);

	//initialize GLEW and check for errors
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}


	glm::vec3 Vertices[3]; //increased the array so that it could contain 3 vertices.
	Vertices[0] = glm::vec3(-0.5f, -0.5f, 0.0f); // down left
	Vertices[1] = glm::vec3(0.5f, -0.5f, 0.0f); // down right
	Vertices[2] = glm::vec3(0.0f, 0.5f, 0.0f); // up


		// buffers
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	createShaders(vertex, fragment);

	glutMainLoop(); //transferring control to GLUT, it's waiting for events in the window

	return 0;
}