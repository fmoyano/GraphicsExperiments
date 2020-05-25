#include "FrancisECS.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <future>
#include <chrono> 

using namespace FrancisECS;

//Define here the behaviour of the program
//If instancing and batching are set to false,
//each quad is drawn in a different draw call
extern const int numInstances = 2000;
constexpr bool instancing = false;
constexpr bool dynamicBatch = true;

GLFWwindow* FrancisECS::window;
std::vector<SpriteRendererComponent> FrancisECS::spriteRenderers;
std::vector<GameEntity> FrancisECS::gameEntities;
std::vector<TransformComponent> FrancisECS::transforms;

GLuint vertexBufferObject;
GLuint instancedVertexBufferPositionObject;
GLuint instancedVertexBufferColorObject;
GLuint elementBufferObject;
GLuint vertexArrayObject;
float *verticesBatch;
unsigned int *indicesBatch;
size_t totalVerticesInfo;
size_t totalIndices;

GLuint defaultSpriteShaderProgram;
GLuint instancedSpriteShaderProgram;

constexpr float verticesPos[] =
{
	0.1f, 0.1f, 0.0f, 1.0f,
	0.1f, -0.1f, 0.0f, 1.0f,
	-0.1f, -0.1f, 0.0f, 1.0f,
	-0.1f, 0.1f, 0.0f, 1.0f
};

constexpr Matrix4x4 basePositionMatrix =
{
	0.1f, 0.1f, 0.0f, 1.0f,
	0.1f, -0.1f, 0.0f, 1.0f,
	-0.1f, -0.1f, 0.0f, 1.0f,
	-0.1f, 0.1f, 0.0f, 1.0f
};

constexpr unsigned int indices[] = {
	0, 1, 2,
	0, 2, 3
};

constexpr unsigned int numVerticesInQuad = 4;
constexpr unsigned int numVertexPositionComponents = 4;
constexpr unsigned int numVertexColorComponents = 3;
constexpr unsigned int totalVertexComponents = numVertexPositionComponents + numVertexColorComponents;
constexpr unsigned long long totalComponentsInMatrix = numVerticesInQuad * totalVertexComponents;
constexpr unsigned int numIndicesPerSprite = 6;

bool once = false;

std::string ReadShaderSource(const std::string& path)
{
	std::string result = "";

	std::ifstream shaderFile;
	shaderFile.open(path.c_str(), std::ifstream::in);

	if (!shaderFile.is_open())
	{
		fprintf(stderr, "Problem reading file %s\n", path.c_str());
	}
	else
	{
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();;
		result = shaderStream.str();
		shaderFile.close();
	}

	return result;
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

GLuint InitializeShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::vector<GLuint> shaderList;

	std::string vertexSource = ReadShaderSource(vertexShaderPath);
	std::string fragmentSource = ReadShaderSource(fragmentShaderPath);

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, vertexSource));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, fragmentSource));

	auto program = CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	return program;
}

void FrancisECS::Init(int width, int height, const char* title)
{
	if (!glfwInit())
	{
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		exit(-1);
	}

	int windowWidth, windowHeight;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);

	glGenBuffers(1, &vertexBufferObject);
	glGenBuffers(1, &elementBufferObject);
	glGenVertexArrays(1, &vertexArrayObject);

	defaultSpriteShaderProgram = InitializeShaderProgram("Shaders\\DefaultSpriteVertexShader.glsl",
		"Shaders\\DefaultSpriteFragmentShader.glsl");

	instancedSpriteShaderProgram = InitializeShaderProgram("Shaders\\DefaultSpriteVertexShaderInstanced2.glsl",
		"Shaders\\DefaultSpriteFragmentShader.glsl");

}

EntityHandle FrancisECS::CreateGameEntity(Vector3 position, decimal rotation, Vector3 scale)
{
	static EntityHandle handle = 0;

	TransformComponent transform = 
	{
		position, rotation, scale, handle
	};

	SpriteRendererComponent spriteRenderer =
	{
		0, 0, 0, 0, 0, Vector3(0, 0, 0), handle
	};
	
	GameEntity e = {
		0,
		handle,
		-1
	};

	transforms.push_back(transform);
	gameEntities.push_back(e);

	return handle++;
}

void FrancisECS::Terminate()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void FrancisECS::AddSpriteRendererComponent(EntityHandle entityHandle, const std::string &texturePath, const Vector3& color)
{
	SpriteRendererComponent sprRenderer;
	
	sprRenderer.entity = entityHandle;
	sprRenderer.color = color;

	if (!instancing && !dynamicBatch)
	{
		glGenBuffers(1, &sprRenderer.vertexBufferObject);
		glGenBuffers(1, &sprRenderer.elementBufferObject);
		//IMPORTANT: OpenGL Core needs Vertex Array Objects to render anything
		glGenVertexArrays(1, &sprRenderer.vertexArrayObject);

		glBindVertexArray(sprRenderer.vertexArrayObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprRenderer.elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		float vertexInfo[numVerticesInQuad * (numVertexPositionComponents + numVertexColorComponents)];

		int counter = 0;
		auto colorArray = glm::value_ptr(sprRenderer.color);
		for (int i = 0; i < numVertexPositionComponents; ++i)
		{
			memcpy(vertexInfo + counter,
				verticesPos + i * numVertexPositionComponents, sizeof(float) * numVertexPositionComponents);
			counter += numVertexPositionComponents;

			memcpy(vertexInfo + counter, colorArray, sizeof(float) * numVertexColorComponents);
			counter += numVertexColorComponents;
		}

		glBindBuffer(GL_ARRAY_BUFFER, sprRenderer.vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexInfo), vertexInfo, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		//glVertexAttribPointer takes the latest buffer bound to GL_ARRAY_BUFFER target
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
	}

	spriteRenderers.push_back(sprRenderer);
	gameEntities[entityHandle].spriteRenderer = spriteRenderers.size();
}

void AddVerticesInfoToBatch(float *verticesBatch, const float* transformedVerticesPositions, const float* sprColor, unsigned int sprIndex) noexcept
{
	int counter = 0;
	for (int i = 0; i < 4; ++i)
	{
		memcpy(verticesBatch + static_cast<unsigned long long>(sprIndex)* totalComponentsInMatrix + counter, transformedVerticesPositions + i * numVertexPositionComponents, numVertexPositionComponents * sizeof(float));
		counter += numVertexPositionComponents;
		memcpy(verticesBatch + static_cast<unsigned long long>(sprIndex)* totalComponentsInMatrix + counter, sprColor, numVertexColorComponents * sizeof(float));
		counter += numVertexColorComponents;
	}
}

void AddIndicesToBatch(unsigned int* indicesBatch, unsigned int sprIndex) noexcept
{
	for (int i = 0; i < numIndicesPerSprite; ++i)
	{
		indicesBatch[numIndicesPerSprite * sprIndex + i] = indices[i] + 4 * sprIndex;
	}
}

void BatchSprites()
{
	//Create the aggregate vertices array
	totalVerticesInfo = totalComponentsInMatrix * spriteRenderers.size();
	verticesBatch = new float[totalVerticesInfo];

	//Create the aggregate indices array
	totalIndices = 6 * spriteRenderers.size();
	indicesBatch = new unsigned int[totalIndices];

	unsigned int sprIndex = 0;
	for (const auto& sprRenderer : spriteRenderers)
	{
		AddIndicesToBatch(indicesBatch, sprIndex);

		auto entity = gameEntities[sprRenderer.entity];
		auto transform = transforms[entity.transform];

		Matrix4x4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::rotate(model, glm::radians(transform.rotation), Vector3(0.0, 0.0, 1.0));
		model = glm::scale(model, transform.scale);

		auto transformedVerticesPositions = glm::value_ptr(model * basePositionMatrix);
		auto sprColor = glm::value_ptr(sprRenderer.color);

		AddVerticesInfoToBatch(verticesBatch, transformedVerticesPositions, sprColor, sprIndex);
		
		++sprIndex;
	}
}

void GenerateInstancedVBO()
{
	if (!once)
	{
		once = true;

		glBindVertexArray(vertexArrayObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPos), verticesPos, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

		glGenBuffers(1, &instancedVertexBufferPositionObject);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVertexBufferPositionObject);
		for (int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(1 + i);
			glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4x4), (const void*)(sizeof(float) * i * 4));
			glVertexAttribDivisor(1 + i, 1);
		}

		glGenBuffers(1, &instancedVertexBufferColorObject);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVertexBufferColorObject);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
		glVertexAttribDivisor(5, 1);
	}

	Matrix4x4 modelArray[numInstances];
	Vector3 colorArray[numInstances];

	int index = 0;
	for (const auto& sprRenderer : spriteRenderers)
	{
		auto entity = gameEntities[sprRenderer.entity];
		auto transform = transforms[entity.transform];

		Matrix4x4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::rotate(model, glm::radians(transform.rotation), Vector3(0.0, 0.0, 1.0));
		model = glm::scale(model, transform.scale);

		modelArray[index] = model;
		colorArray[index] = sprRenderer.color;
		++index;
	}	

	glBindBuffer(GL_ARRAY_BUFFER, instancedVertexBufferPositionObject);
	glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(Matrix4x4), modelArray, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, instancedVertexBufferColorObject);
	glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(Vector3), colorArray, GL_DYNAMIC_DRAW);
}

void RunSpriteRendererSystem()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (instancing)
	{
		GenerateInstancedVBO();
		glUseProgram(instancedSpriteShaderProgram);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, spriteRenderers.size());
	}
	else if (dynamicBatch)
	{
		BatchSprites();

		glBindVertexArray(vertexArrayObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * totalIndices, indicesBatch, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * totalVerticesInfo, verticesBatch, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		//glVertexAttribPointer takes the latest buffer bound to GL_ARRAY_BUFFER target
		glVertexAttribPointer(0, numVertexPositionComponents, GL_FLOAT, GL_FALSE, totalVertexComponents * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, numVertexColorComponents, GL_FLOAT, GL_FALSE, totalVertexComponents * sizeof(float), (void*)(numVertexPositionComponents * sizeof(float)));

		delete[] verticesBatch;
		delete[] indicesBatch;
		
		glUseProgram(defaultSpriteShaderProgram);

		glUniformMatrix4fv(glGetUniformLocation(defaultSpriteShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glUseProgram(defaultSpriteShaderProgram);

		for (const auto& sprRenderer : spriteRenderers)
		{
			auto entity = gameEntities[sprRenderer.entity];
			auto transform = transforms[entity.transform];

			Matrix4x4 model = glm::mat4(1.0f);
			model = glm::translate(model, transform.position);
			model = glm::rotate(model, glm::radians(transform.rotation), Vector3(0.0, 0.0, 1.0));
			model = glm::scale(model, transform.scale);

			glUniformMatrix4fv(glGetUniformLocation(defaultSpriteShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(sprRenderer.vertexArrayObject);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
}



void RunTransformSystem(float deltaTime)
{
	for (auto& transform : transforms)
	{
		transform.rotation += 100.0f * deltaTime;
	}
}

void FrancisECS::RunSystems(float deltaTime)
{
	//Run rest of systems...
	//...

	RunTransformSystem(deltaTime);

	//Run sprite renderer system
	RunSpriteRendererSystem();
}
