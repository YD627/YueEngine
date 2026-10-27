#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

out vec3 v_Color;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_WorldPosition;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 worldPosition = transform * vec4(aPos, 1.0);

	gl_Position = projection * view * worldPosition;
	v_Color = aColor;
	v_Normal = mat3(transpose(inverse(transform))) * aNormal;
	v_TexCoord = aTexCoord;
	v_WorldPosition = worldPosition.xyz;
}
