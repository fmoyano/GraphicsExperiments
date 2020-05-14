#ifndef FRANCISECS_H
#define FRANCISECS_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace FrancisECS
{
	using EntityHandle = __int32;
	using ComponentHandle = __int32;
	using Vector4 = glm::vec4;
	using Vector3 = glm::vec3;
	using Matrix4x4 = glm::mat4;
	using decimal = float;

	typedef struct
	{
		Vector3 position;
		decimal rotation;
		Vector3 scale;
		EntityHandle entity;
	} TransformComponent;

	extern std::vector<TransformComponent> transforms;

	typedef struct
	{
		GLuint vertexBufferObject;
		GLuint vertexInstanceBufferObject;
		GLuint elementBufferObject;
		GLuint vertexArrayObject;
		GLuint texture;
		Vector3 color;
		EntityHandle entity;
	} SpriteRendererComponent;

	extern std::vector<SpriteRendererComponent> spriteRenderers;

	typedef struct
	{
		EntityHandle entity;
		ComponentHandle transform;
		ComponentHandle spriteRenderer;
	} GameEntity;

	extern std::vector<GameEntity> gameEntities;

	extern GLFWwindow* window;

	void Init(int width, int height, const char* title);
	void Terminate();
	EntityHandle CreateGameEntity(Vector3 position = Vector3(0.0, 0.0, 0.0), decimal rotation = 0.0, Vector3 scale = Vector3(1.0, 1.0, 1.0));
	void AddSpriteRendererComponent(EntityHandle entity, const std::string& texturePath, const Vector3& color = Vector3(1.0, 1.0, 1.0));
	void RunSystems(float deltaTime);
}

#endif // !FRANCISECSS_H
