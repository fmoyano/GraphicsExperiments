#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in mat4 model;
layout (location = 5) in vec3 color;

out vec3 outColor;

void main()
{
   outColor = color;
   gl_Position = model * position;
}
