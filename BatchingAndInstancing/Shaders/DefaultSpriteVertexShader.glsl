#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 color;

uniform mat4 model;

out vec3 outColor;

void main()
{
   outColor = color;
   gl_Position = model * position;
}
