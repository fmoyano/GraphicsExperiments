#version 330 core

layout (location = 0) in vec4 position;

uniform mat4 model[100];
uniform vec3 color[100];

out vec3 outColor;

void main()
{
   outColor = color[gl_InstanceID];
   gl_Position = model[gl_InstanceID] * position;
}
