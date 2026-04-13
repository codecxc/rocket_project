#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Исходный код вершинного шейдера
const GLchar* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}
)";

// Исходный код фрагментного шейдера
const GLchar* fragmentShaderSource = R"(
#version 330 core

out vec4 color;

void main()
{
    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

int main() {
	glfwInit(); // инициализация GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // версия 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // версия 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // используем Core Profile
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // отключить изменение размеров окна
	
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle", nullptr, nullptr); // создание окна
	if (window == nullptr) {return -1;}
	
	glfwMakeContextCurrent(window); // делаем окно текущим контекстом OpenGL
	
	//glewExperimental = GL_TRUE; // включаем современные функции OpenGL
	
	// ИСПРАВЛЕНИЕ: не выходим сразу при ошибке GLEW, пробуем продолжить
	GLenum glewError = glewInit(); // инициализация GLEW
	if (glewError != GLEW_OK) {
		// GLEW может выдать ошибку, но всё равно работать
		// поэтому не выходим сразу
	}
	
	// Очищаем возможные ошибки OpenGL после glewInit
	glGetError();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height); // получаем размеры фреймбуфера
	glViewport(0, 0, width, height); // устанавливаем область отрисовки

	// Координаты вершин треугольника
	GLfloat vertices[] = {
   		-0.5f, -0.5f, 0.0f,
     		0.5f, -0.5f, 0.0f,
     		0.0f,  0.5f, 0.0f
	};

	// === ПРАВИЛЬНЫЙ ПОРЯДОК: сначала VAO, потом VBO ===
	
	GLuint VAO; // Vertex Array Object - хранит конфигурацию вершинных атрибутов
	glGenVertexArrays(1, &VAO); // создание VAO
	glBindVertexArray(VAO); // привязка VAO (все последующие настройки будут сохранены в нём)

	GLuint VBO; // Vertex Buffer Object - буфер для вершинных данных на GPU
	glGenBuffers(1, &VBO); // создание буфера
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // привязка буфера как массива вершин
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // загрузка данных в буфер
	// GL_STATIC_DRAW: данные либо никогда не будут изменяться, либо будут изменяться очень редко
	// GL_DYNAMIC_DRAW: данные будут меняться довольно часто
	// GL_STREAM_DRAW: данные будут меняться при каждой отрисовке

	// Настройка интерпретации вершинных данных
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	// Параметры: индекс атрибута, количество компонент, тип данных, нормализация, шаг, смещение
	glEnableVertexAttribArray(0); // включаем вершинный атрибут с индексом 0

	glBindVertexArray(0); // отвязываем VAO (защита от случайных изменений)

	// === ШЕЙДЕРЫ ===
	
	GLuint vertexShader; // идентификатор вершинного шейдера
	vertexShader = glCreateShader(GL_VERTEX_SHADER); // создание вершинного шейдера
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // загрузка исходного кода шейдера
	glCompileShader(vertexShader); // компиляция шейдера

	GLuint fragmentShader; // идентификатор фрагментного шейдера
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // создание фрагментного шейдера
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // загрузка исходного кода
	glCompileShader(fragmentShader); // компиляция шейдера

	// Создание шейдерной программы
	GLuint shaderProgram;
	shaderProgram = glCreateProgram(); // создание объекта программы
	glAttachShader(shaderProgram, vertexShader); // присоединение вершинного шейдера
	glAttachShader(shaderProgram, fragmentShader); // присоединение фрагментного шейдера
	glLinkProgram(shaderProgram); // линковка программы

	glDeleteShader(vertexShader); // удаление шейдера (после линковки он уже не нужен)
	glDeleteShader(fragmentShader); // удаление шейдера

	// Основной цикл отрисовки
	while(!glfwWindowShouldClose(window)) {
    		glfwPollEvents(); // обработка событий (клавиатура, мышь и т.д.)

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // установка цвета очистки (тёмно-бирюзовый)
		glClear(GL_COLOR_BUFFER_BIT); // очистка буфера цвета
		
		glUseProgram(shaderProgram); // активация шейдерной программы
		glBindVertexArray(VAO); // привязка VAO с настройками вершин
		glDrawArrays(GL_TRIANGLES, 0, 3); // отрисовка 3 вершин как треугольник
		// Параметры: примитив, начальный индекс, количество вершин
		glBindVertexArray(0); // отвязка VAO
		
    		glfwSwapBuffers(window); // обмен буферов (double buffering)
	}

	// Очистка ресурсов
	glDeleteVertexArrays(1, &VAO); // удаление VAO
	glDeleteBuffers(1, &VBO); // удаление VBO
	glDeleteProgram(shaderProgram); // удаление шейдерной программы

	glfwTerminate(); // завершение работы GLFW
	return 0;
}
