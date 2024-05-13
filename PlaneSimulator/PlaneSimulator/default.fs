#version 330 core

in vec3 vs_FragPos;
in vec3 vs_Color;
in vec2 vs_TexCoord;
in vec3 vs_Normal;
in vec3 vs_Ambient;
in vec3 vs_Diffuse;
in vec3 vs_Specular;
out vec4 FragColor;


uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform int n = 1;

void main()
{
	vec3 ambiental = (vec4(lightColor, 1.f) * vec4(vs_Ambient, 1.f)).xyz;

	vec3 lightDir = normalize(vs_FragPos - lightPos);
	vec3 viewDir = normalize(viewPos - vs_FragPos);
	vec3 reflectDir = reflect(lightDir, vs_Normal);
	float specFactor = pow(max(dot(viewDir, reflectDir), 0.0f), n);
	vec3 diffuse = lightColor * vs_Diffuse * clamp(dot(lightDir, vs_Normal), 0, 1);
	vec3 specular = vs_Specular.x * specFactor * (lightColor - vec3(0.3f));

	FragColor = (vec4((diffuse + ambiental), 1.0f) + 0.2f * vec4(specular, 1.0f)) * vec4(vs_Color, 1.0f) * 0.8f + 0.2f * vec4(specular, 1.0f);
}