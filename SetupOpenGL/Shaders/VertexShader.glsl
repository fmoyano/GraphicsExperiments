#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 projection;

out vec3 outColor;
out vec2 outTexCoord;

void main()
{
   outColor = color;
   outTexCoord = texCoord;
   gl_Position = projection * model * position;
}
