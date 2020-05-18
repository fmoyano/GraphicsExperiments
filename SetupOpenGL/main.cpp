#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint shaderProgram;
GLuint vertexBufferObject;
GLuint texcoordBufferObject;
GLuint elementBufferObject;
GLuint vertexArrayObject;

GLuint texture;
GLuint texture2;

int width, height;

constexpr float vertices[] = {
	0.2f, 0.2f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.2f, -0.8f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	-0.8f, -0.8f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	-0.8f, 0.2f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.8f, -0.2f, 0.8f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	-0.2f, -0.2f, 0.8f, 1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	-0.2f, 0.8f, 0.8f, 1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,

	0.6f, 0.6f, 0.6f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	0.6f, -0.4f, 0.6f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	-0.4f, -0.4f, 0.6f, 1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	-0.4f, 0.6f, 0.6f, 1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
};

constexpr float vertices2[] = {
	0.2f, 0.2f, 0.0f, 1.0f,
	0.2f, -0.8f, 0.0f, 1.0f, 
	-0.8f, -0.8f, 0.0f, 1.0f,
	-0.8f, 0.2f, 0.0f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, -0.2f, 0.8f, 1.0f,
	-0.2f, -0.2f, 0.8f, 1.0f,
	-0.2f, 0.8f, 0.8f, 1.0f,

	0.6f, 0.6f, 0.6f, 1.0f,
	0.6f, -0.4f, 0.6f, 1.0f,
	-0.4f, -0.4f, 0.6f, 1.0f,
	-0.4f, 0.6f, 0.6f, 1.0f,
};

constexpr float texcoords[] = {
	 1.0f, 1.0f,
	 1.0f, 0.0f,
	 0.0f, 0.0f,
	 0.0f, 1.0f,

	 1.0f, 1.0f,
	 1.0f, 0.0f,
	 0.0f, 0.0f,
	 0.0f, 1.0f,

	 1.0f, 1.0f,
	 1.0f, 0.0f,
	 0.0f, 0.0f,
	 0.0f, 1.0f
};

constexpr unsigned int indices[] = {
	0, 1, 2,
	0, 2, 3,

	4, 5, 6,
	4, 6, 7,

	8, 9, 10,
	8, 10, 11
};

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void FramebufferSizeChanged(GLFWwindow* window, int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;
	glViewport(0, 0, width, height);
}

//This version of Initialize uses one unique VBO to store all the vertex attributes
void Initialize1()
{
	glGenBuffers(1, &vertexBufferObject);
	glGenBuffers(1, &elementBufferObject);
	//IMPORTANT: OpenGL Core needs Vertex Array Objects to render anything
	glGenVertexArrays(1, &vertexArrayObject);

	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	//glVertexAttribPointer takes the latest buffer bound to GL_ARRAY_BUFFER target
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(4*sizeof(float)));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
}

//This version of Initialize uses two VBOs: one stores positions, the other stores texture coordinates
void Initialize2()
{
	glGenBuffers(1, &vertexBufferObject);
	glGenBuffers(1, &elementBufferObject);
	glGenBuffers(1, &texcoordBufferObject);
	//IMPORTANT: OpenGL Core needs Vertex Array Objects to render anything
	glGenVertexArrays(1, &vertexArrayObject);
	
	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer takes the latest buffer bound to GL_ARRAY_BUFFER target
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordBufferObject);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}

//This version of Initialize uses binding points to decouple
//vertex format from source data (available since OpenGL 4.6)
void Initialize3()
{
	//IMPORTANT: OpenGL Core needs Vertex Array Objects to render anything
	glGenVertexArrays(1, &vertexArrayObject);

	glCreateBuffers(1, &vertexBufferObject);
	glCreateBuffers(1, &elementBufferObject);

	glBindVertexArray(vertexArrayObject);

	glNamedBufferData(vertexBufferObject, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glNamedBufferData(elementBufferObject, sizeof(indices), indices, GL_STATIC_DRAW);

	//Set data for binding point 0
	//Buffer binding point aggregates source buffer object, byte offset and stride, and instance divisor
	glBindVertexBuffer(0, vertexBufferObject, 0, 9 * sizeof(float));
	glVertexBindingDivisor(0, 0); //all vertex attributes associated with this binding will use the same divisor

	//Vertex format aggregates enabled/disabled attributes, size type and normalization of vertex attribute data...
	//... buffer binding point associated, byte offset
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float));
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float));
	
	//Associate vertex attribute with binding
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
}

GLuint CreateShader(GLenum shaderType, const std::string& strShaderFile)
{
	GLuint shader = glCreateShader(shaderType);
	const char* strFileData = strShaderFile.c_str();

	glShaderSource(shader, 1, &strFileData, nullptr);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (GL_FALSE == status)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[static_cast<size_t>(infoLogLength) + 1ULL];
		glGetShaderInfoLog(shader, infoLogLength, nullptr, strInfoLog);

		const char* strShaderType = nullptr;
		switch (shaderType)
		{
		case GL_VERTEX_SHADER:
			strShaderType = "vertex";
			break;

		case GL_GEOMETRY_SHADER:
			strShaderType = "geometry";
			break;

		case GL_FRAGMENT_SHADER:
			strShaderType = "fragment";
			break;
		}

		fprintf(stderr, "Compile failure in %s shader: \n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint>& shaderList)
{
	GLuint program = glCreateProgram();

	for (auto shader : shaderList)
	{
		glAttachShader(program, shader);
	}

	glLinkProgram(program);
	
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (GL_FALSE == status)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[static_cast<size_t>(infoLogLength) + 1ULL];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (auto shader : shaderList)
	{
		glDetachShader(program, shader);
	}

	return program;
}

std::string ReadShaderSource(const std::string& path)
{
	std::ifstream shaderFile;
	shaderFile.open(path.c_str());

	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();;
	
	shaderFile.close();

	return shaderStream.str();
}

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	std::string vertexSource = ReadShaderSource("Shaders\\VertexShader.glsl");
	std::string fragmentSource = ReadShaderSource("Shaders\\FragmentShader.glsl");

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, vertexSource));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, fragmentSource));

	shaderProgram = CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void LoadTexture()
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("Assets\\container.jpg", &width, &height, &nrChannels, 0);

	if (!data)
	{
		exit(-1);
	}

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	data = stbi_load("Assets\\awesomeface.png", &width, &height, &nrChannels, 0);
	if (!data)
	{
		exit(-1);
	}

	glGenTextures(1, &texture2);
	
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	//Here we're saying that "texture" will be on texture unit 0 and "texture2" on texture unit 1...
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//...And here we're connecting the shader samplers to the texture units
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "myTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "myTexture2"), 1);
}

void Update(double time)
{
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width)/static_cast<float>(height), 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::rotate(model,static_cast<float>(time), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

int main()
{
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(640, 480, "Hello OpenGL", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeChanged);
	
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		return -1;
	}

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	//Different versions of Initialize: they all work
	//Initialize1();
	//Initialize2();
	Initialize3();
	
	InitializeProgram();
	LoadTexture();
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

		Update(glfwGetTime());
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}