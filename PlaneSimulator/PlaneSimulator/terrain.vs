#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aAmbient;
layout(location = 5) in vec3 aDiffuse;
layout(location = 6) in vec3 aSpecular;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}