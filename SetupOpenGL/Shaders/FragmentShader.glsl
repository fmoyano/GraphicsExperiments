#version 330 core

uniform sampler2D myTexture;
uniform sampler2D myTexture2;

in vec3 outColor;
in vec2 outTexCoord;

out vec4 FragColor;

void main()
{
    FragColor = mix(texture(myTexture, outTexCoord), texture(myTexture2, outTexCoord), 0.2);
    //FragColor = texture(myTexture, outTexCoord) * vec4(outColor, 1.0);
}
