#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec3 aAmbient;
layout(location = 5) in vec3 aDiffuse;
layout(location = 6) in vec3 aSpecular;

out vec3 vs_FragPos;
out vec3 vs_Color;
out vec2 vs_TexCoord;
out vec3 vs_Normal;
out vec3 vs_Ambient;
out vec3 vs_Diffuse;
out vec3 vs_Specular;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_FragPos = vec4(model * vec4(aPos, 1.0f)).xyz;
	vs_Color = aColor;
	vs_TexCoord = vec2(aTexCoord.x, aTexCoord.y * -1.f);
	vs_Normal = mat3(model) * aNormal;
	vs_Ambient = aAmbient;
	vs_Diffuse = aDiffuse;
	vs_Specular = aSpecular;

	gl_Position = projection * view * vec4(vs_FragPos, 1.0f);
}
