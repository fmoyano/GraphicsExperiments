#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "FrancisECS.h"

extern const int numInstances;
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
	glViewport(0, 0, newWidth, newHeight);
}

static void ProcessInput(GLFWwindow* window)
{

}

double lastTime;
int main()
{
	FrancisECS::Init(640, 480, "My prototype");

	glfwSetKeyCallback(FrancisECS::window, KeyCallback);
	glfwSetFramebufferSizeCallback(FrancisECS::window, FramebufferSizeChanged);

	auto numInstancePerColumn = static_cast<int>(sqrt(numInstances));
	for (int i = 0; i < numInstancePerColumn; ++i)
	{
		for (int j = 0; j < numInstancePerColumn; ++j)
		{
			auto entityHandle = FrancisECS::CreateGameEntity(FrancisECS::Vector3(-1.0 + (float)i * 0.1, 1.0 - (float)j * 0.1, 0.0), 0.0, FrancisECS::Vector3(0.5f, 0.5f, 1.0f));
			FrancisECS::AddSpriteRendererComponent(entityHandle, "", FrancisECS::Vector3((float)i/11.0, 0.0, (float)j / 11.0));
		}
	}
		
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