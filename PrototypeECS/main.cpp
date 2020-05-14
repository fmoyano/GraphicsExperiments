#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
/*#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>*/

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "FrancisECS.h"

GLuint texture;
GLuint texture2;

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void FramebufferSizeChanged(GLFWwindow* window, int newWidth, int newHeight)
{
	/*width = newWidth;
	height = newHeight;*/
	glViewport(0, 0, newWidth, newHeight);
}

static void ProcessInput(GLFWwindow* window)
{

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
	/*glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "myTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "myTexture2"), 1);*/
}

/*void Update(double time)
{
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width)/static_cast<float>(height), 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	model = glm::rotate(model,static_cast<float>(time), glm::vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
}*/

double lastTime;

int main()
{
	FrancisECS::Init(640, 480, "My prototype");

	glfwSetKeyCallback(FrancisECS::window, KeyCallback);
	glfwSetFramebufferSizeCallback(FrancisECS::window, FramebufferSizeChanged);

	for (int i = 0; i < 21; ++i)
	{
		for (int j = 0; j < 21; ++j)
		{
			auto entityHandle = FrancisECS::CreateGameEntity(FrancisECS::Vector3(-1.0 + (float)i * 0.1, 1.0 - (float)j * 0.1, 0.0), 0.0, FrancisECS::Vector3(0.5f, 0.5f, 1.0f));
			FrancisECS::AddSpriteRendererComponent(entityHandle, "", FrancisECS::Vector3((float)i/11.0, 0.0, (float)j / 11.0));
		}
	}
	
	//LoadTexture();
	
	lastTime = 0.0;
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	while (!glfwWindowShouldClose(FrancisECS::window))
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		ProcessInput(FrancisECS::window);

		FrancisECS::RunSystems(deltaTime);
		
		glfwPollEvents();
		glfwSwapBuffers(FrancisECS::window);
	}

	FrancisECS::Terminate();
	
	return 0;
}