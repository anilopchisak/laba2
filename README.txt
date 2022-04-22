1) Индексный буфер
	GLuint IBO;

2) GLuint gWorldLocation;
указатель для доступа к всемирной матрице, представленной в виде uniform-переменной внутри 
шейдера. Всемирная она потому, что всё что мы делаем с объектом, это изменение его позиции 
в место, которое мы указываем относительно координатной системы внутри нашего виртуального 
'мира'.

3) Переменная, которая увеличивается каждый вызов функции рендера
	float Scale = 0.0f;

4) Переменная, необходимая для проверки успеха компиляции шейдера/шейдерной программы
	GLint success; 

5) Массив символов, нужен для отображения ошибок компиляции
	GLchar InfoLog[1024];

6) Установка ширины и высоты окна
	constexpr auto WINDOW_WIDTH = 1980;
	constexpr auto WINDOW_HEIGHT = 1250;

7)Вершинный шейдер
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

	1. Использование GLSL версии 3.3
		#version 330 core

	2. Координатные переменные в атрибуте идут первыми среди всех остальных данных, 
	поэтому указываем для них location = 0
		layout (location = 0) in vec3 Pos;
	
	3. Определяем выходную переменную, задающую цвет, которую затем отправим 
	во фрагментный шейдер
		out vec4 vertexColor;

	4. Uniform-переменная типа матрицы 4x4
		uniform mat4 gWorld;

	5. Умножаем полученную матрицу на вектор
		gl_Position = gWorld * vec4(4 * Pos.x, 4 * Pos.y, Pos.z, 1.0);

	6. Задает цвет фигуры
		vertexColor = vec4(clamp(Pos, 0.0, 1.0), 1.0);
	Функция clamp(), которая проверит, не выходят ли наши значения из промежутка 0.0-1.0.

8)Фрагментный шейдер
	static const char* fragment = R"(
		#version 330 core
		out vec4 FragColor;
		in vec4 vertexColor;
		void main()
		{
			FragColor = vertexColor;
		})";


9) Функция для проверки на ошибки компилирования шейдеров и шейдерной программы.
Получает статус компиляции и отображает все ошибки, обнаруженные компилятором.
	void checkerror(GLuint program, GLint success, GLenum ShaderType)
	{
			if (!success)
		{
			if (ShaderType == 0) // шейдерная программа
			{
				glGetProgramInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
				std::cerr << "Error compiling shader type " << static_cast<int>(ShaderType) << InfoLog << "\n";
			}
			else // шейдеры
			{
				glGetShaderInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
				std::cerr << "Error linking shader program " << InfoLog << "\n";
			}	
		}
	}

	Функция, которая возвращает информацию по объекту
		glGetProgramInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
	

10) Функция для создания объекта шейдера и добавления его в шейдерную программу
	void addshader(GLuint ShaderProgram, const char* ShaderText, GLenum ShaderType)
	{
		GLuint shader = glCreateShader(ShaderType);

		const GLchar* ShaderSource[1]; //принимает тип шейдера как параметр
		ShaderSource[0] = ShaderText;
		glShaderSource(shader, 1, ShaderSource, nullptr);
		glCompileShader(shader);

		// Checking for vertex shader compilation errors
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		checkerror(shader, success, ShaderType);

		glAttachShader(ShaderProgram, shader);
	}

	1. Создание шейдера определенного типа
		GLuint shader = glCreateShader(ShaderType);

	2. Получает исходный код шейдера
		const GLchar* ShaderSource[1]; 
		ShaderSource[0] = ShaderText;
		glShaderSource(shader, 1, ShaderSource, nullptr);
		// созданный шейдер, количество, исходный код шейдера, массив длин строк

	3. Компилирование шейдера
		glCompileShader(shader);

	4. Проверка ecgt[f компиляции шейдера
	Возвращается в success GL_TRUE, если компляция была успешной, иначе GL_FALSE
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		checkerror(shader, success, ShaderType);

	5. Присоединяем скомпилированный объект шейдера к объекту программы
		glAttachShader(ShaderProgram, shader);

11) Создание шейдеров
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

	1. Создание программного объекта (программы шейдеров)
		GLuint ShaderProgram = glCreateProgram();

	2. Создание шейдеров и добавление их в программный объект
		addshader(ShaderProgram, ShaderText_v, GL_VERTEX_SHADER);
		addshader(ShaderProgram, ShaderText_f, GL_FRAGMENT_SHADER);
	
	3. Линковка шейдеров - позволяет драйверу урезать и оптимизировать шейдеры
		glLinkProgram(ShaderProgram);

	4. Проверка на ошибки компилирования
		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
		checkerror(ShaderProgram, success, 0);

	5. Назначаем программный объект для использования конвейера - эта программа сохранит параметры 
	для всех вызовов отрисовки, пока не будет заменена на другую или не будет запрещено
	ее использование напрямую функцией glUseProgram с параметром nullptr
		glUseProgram(ShaderProgram);

	6. Запрашиваем доступ к индексу позиции uniform-переменной gWorld в программном объекте
		// функция с указателем на объект программы и именем переменной
		gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");

		// проверка на ошибки (иначе обновления переменной не попадут в шейдер)
		assert(gWorldLocation != 0xFFFFFFFF);

12) Изменения в функции RenderSceneCB
	1. Переменная float, которая увеличивается каждый вызов функции рендера
		Scale += 0.1f;

	2. Создаем объект класса Pipeline и с помощью методов класса задаем значения 
	атрибутов класса
		Pipeline p;
		p.Scale(0.1f, 0.1f, 0.1f);
		p.WorldPos(0.0f, 0.0f, 3.0f);
		p.Rotate(0, Scale, 0);
		glm::vec3 CameraPos(0.0f, 0.0f, -3.0f);
		glm::vec3 CameraTarget(0.0f, 0.0f, 2.0f);
		glm::vec3 CameraUp(0.0f, 1.0f, 0.0f);
		p.SetCamera(CameraPos, CameraTarget, CameraUp);
		p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

	3. Функция glUniform для загрузки данных в uniform-переменную шейдера gWorld
		glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (GLfloat*)p.GetTrans());

	*gWorldLocation - это адрес uniform-переменной (находится после компиляции шейдера 
	используя glGetUniformLocation()) 
	*Второй параметр - это количество матриц, значения которых обновляются
	*GL_TRUE - указывает подается ли матрица по строковому или по столбиковому порядку
		(По строковая матрица означает, что матрица содержит строки одна за другой) 

13) класс Pipeline

	1. Атрибуты

		struct m_persProj
		{
			float Width; float Height;
			float zNear; float zFar;
			float FOV;
		};

		struct m_camera
		{
			glm::vec3 Pos;
			glm::vec3 Target;
			glm::vec3 Up;
		};


		glm::vec3 m_scale;

		glm::vec3 m_worldPos;

		glm::vec3 m_rotateInfo;

		glm::mat4 m_transformation;

		m_camera camera;

		m_persProj persproj;

	2. С помощью функций Scale, WorldPos, Rotate, SetPerspectiveProj, SetCamera задаются
	значения атрибутов класса.
		
	3. Функция GetTrans создает матрицы и возвращает результат их перемножения

	4. Функции InitScaleTransform (матрица масштабирования), 
	InitRotateTransform (матрица поворота), InitTranslationTransform (матрица переноса)
	создают соответствующие матрицы

	7. Матрица проекции - проецирование перспективы
		glm::mat4 InitPerspectiveProj(float width, float height, float zNear, float zFar, float FOV)
		{
			glm::mat4 m;

			float ar = width / height;
			float zRange = zNear - zFar;
			float tanHalfFOV = tanf(glm::radians(FOV / 2.0f));

			m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;	m[0][2] = 0.0f;		     m[0][3] = 0.0;			 m[0][3] = 0.0;
			m[1][0] = 0.0f;						m[1][1] = 1.0f / tanHalfFOV; m[1][2] = 0.0f;			 m[1][3] = 0.0;
			m[2][0] = 0.0f;						m[2][1] = 0.0f;		     m[2][2] = (-zNear - zFar) / zRange; m[2][3] = 2.0f * zFar * zNear / zRange;
			m[3][0] = 0.0f;						m[3][1] = 0.0f;		     m[3][2] = 1.0f;			 m[3][3] = 0.0;
	
			return m;
		}
		
	8. Функция генерирует преобразования камеры, которые позднее будут использованы конвейером.
	Функция получает вектор направления и верхний вектор.
		glm::mat4 InitCameraTransform(glm::vec3& Target, glm::vec3& Up)
		{
			glm::mat4 m;

			glm::vec3 N = Target;
			N = glm::normalize(N);
			glm::vec3 U = Up;
			U = glm::normalize(U);
			U = glm::cross(U, Target);
			glm::vec3 V = glm::cross(N, U);

			m[0][0] = U.x;  m[0][1] = U.y;  m[0][2] = U.z;  m[0][3] = 0.0f;
			m[1][0] = V.x;  m[1][1] = V.y;  m[1][2] = V.z;  m[1][3] = 0.0f;
			m[2][0] = N.x;  m[2][1] = N.y;  m[2][2] = N.z;  m[2][3] = 0.0f;
			m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
			return m;
		}
