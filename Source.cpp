#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector

GLuint VBO; // a global variable for storing a pointer to the vertex buffer

void RenderSceneCB() //draw
{


	// Рендеринг

	glClear(GL_COLOR_BUFFER_BIT); //clearing the frame buffer using the color specified above

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind (привязывать) the buffer, prepare it for rendering

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // data inside the buffer (index, 
	glEnableVertexAttribArray(0); // Enable or disable the shared (общий) array of vertex attributes

	//glDrawArrays(GL_POINTS, 0, 1);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	// показывает, что текущее окно должно быть отрисованно заново и в течении 
	// главного цикла GLUT функция рендера будет вызвана
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

	//glutDisplayFunc(RenderSceneCB); //GLUT interacts with window system
	glutIdleFunc(RenderSceneCB);

	//initialize GLEW and check for errors
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}


	//glm::vec3 Vertices[1]; //arrays of
	//Vertices[0] = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 Vertices[3]; //increased the array so that it could contain 3 vertices.
	Vertices[0] = glm::vec3(-0.5f, -0.5f, 0.0f); // down left
	Vertices[1] = glm::vec3(0.5f, -0.5f, 0.0f); // down right
	Vertices[2] = glm::vec3(0.0f, 0.5f, 0.0f); // up


		// creating shaders

	GLuint ShaderProgram = glCreateProgram();
	GLint success;
	GLchar InfoLog[1024];

	// vertex shader 
	GLuint ShaderObj_vert = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertexShaderSource[1];
	vertexShaderSource[0] = "#version 330 core\n"
		// координатные переменные в атрибуте идут первыми среди всех остальных данных, 
		// поэтому указываем для них location = 0
		"layout (location = 0) in vec3 Pos;\n"
		// определяем выходную переменную, задающую цвет, 
		// которую затем отправим во фрагментный шейдер
		"out vec4 vertexColor;\n"
		"void main()\n"
		"{\n"
		// * 0.5 - уменьшает иходный размер треугольника
		"   gl_Position = vec4(0.5 * Pos.x, 0.5 * Pos.y, Pos.z, 1.0);\n"
		"   vertexColor = vec4(0.0, 1.0, 1.0, 1.0);\n" // цвет фигуры
		"}\0";
	glShaderSource(ShaderObj_vert, 1, vertexShaderSource, NULL);
	glCompileShader(ShaderObj_vert);

	// Проверка на наличие ошибок компилирования вершинного шейдера
	glGetShaderiv(ShaderObj_vert, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ShaderObj_vert, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", GL_VERTEX_SHADER, InfoLog);
	}

	// fragment shader
	GLuint ShaderObj_frag = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSource[1];
	fragmentShaderSource[0] = "#version 330 core\n"
		//"out vec4 FragColor;\n"
		"uniform vec4 myColor;\n" // выходной цвет фрагментного шейдера зависит от значения данной переменной
		"in vec4 vertexColor;\n" // входная переменная из вершинного шейдера
		"void main()\n"
		"{\n"
		"	FragColor = myColor;\n"
		//"   FragColor = vertexColor;\n"
		"}\n\0";
	glShaderSource(ShaderObj_frag, 1, fragmentShaderSource, NULL);
	glCompileShader(ShaderObj_frag);

	glGetShaderiv(ShaderObj_frag, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ShaderObj_frag, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", GL_FRAGMENT_SHADER, InfoLog);
	}

	// Связывание шейдеров
	glAttachShader(ShaderProgram, ShaderObj_vert);
	glAttachShader(ShaderProgram, ShaderObj_frag);
	glLinkProgram(ShaderProgram);
	// Проверка на наличие ошибок связывания шейдеров
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", InfoLog);
	}
	glUseProgram(ShaderProgram);



		// buffers
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glutMainLoop(); //transferring control to GLUT, it's waiting for events in the window

	glDeleteBuffers(1, &VBO);
	glDeleteShader(ShaderObj_vert);
	glDeleteShader(ShaderObj_frag);

	return 0;
}