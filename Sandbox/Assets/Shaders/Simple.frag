#version 450 core
in vec3 v_Color;
in vec3 v_Normal;
in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec3 u_LightDirection;
uniform sampler2D u_Texture;

void main()
{
	vec3 texColor = texture(u_Texture, v_TexCoord).rgb;
	vec3 tint = mix(vec3(1.0), v_Color, 0.5);

	float diffuse = max(dot(normalize(v_Normal), normalize(-u_LightDirection)), 0.0);
	float ambient = 0.15;

	FragColor = vec4(texColor * tint * (diffuse + ambient), 1.0);
}
