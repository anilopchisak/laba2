#pragma once
#include <iostream>
#include <GL/glew.h> // extensions manager
#include <GL/freeglut.h> //GLUT - OpenGL Utility Library - API for managing the window system, as well as event handling, input/output control
#include <glm/glm.hpp>	//#include "math_3d.h" - vector
class Shader
{
protected:
	GLuint ShaderProgram;
	GLint success;
public:
	Shader()
	{
		GLchar InfoLog[1024];
		ShaderProgram = glCreateProgram();

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
			"uniform mat4 gWorld;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = gWorld * vec4(Pos.x, Pos.y, Pos.z, 1.0);"
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
			"out vec4 FragColor;\n"
			"in vec4 vertexColor;\n" // входная переменная из вершинного шейдера
			"void main()\n"
			"{\n"
			"   FragColor = vertexColor;\n"
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
	}

	GLint return_ShaderProgram() 
	{
		return ShaderProgram;
	}
};

