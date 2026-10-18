#version 450 core
in vec3 v_Color;
in vec3 v_Normal;
out vec4 FragColor;

uniform vec3 u_LightDirection;

void main()
{
	float diffuse = max(dot(normalize(v_Normal), normalize(-u_LightDirection)), 0.0);
	FragColor = vec4(v_Color * diffuse, 1.0);
}
