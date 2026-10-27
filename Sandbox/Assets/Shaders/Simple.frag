#version 450 core

in vec3 v_Color;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_WorldPosition;

out vec4 FragColor;

uniform vec3 u_CameraPosition;

uniform vec3 u_LightDirection;
uniform vec3 u_LightColor;

uniform vec3 u_PointLightPosition;
uniform vec3 u_PointLightColor;

uniform vec3 u_AmbientColor;
uniform float u_SpecularStrength;
uniform float u_Shininess;

uniform sampler2D u_Texture;

void main()
{
	vec3 N = normalize(v_Normal);
	vec3 V = normalize(u_CameraPosition - v_WorldPosition);

	vec3 texColor = texture(u_Texture, v_TexCoord).rgb;
	vec3 albedo = texColor * mix(vec3(1.0), v_Color, 0.5);

	vec3 color = albedo * u_AmbientColor;

	// Directional light (Blinn-Phong).
	vec3 L = normalize(-u_LightDirection);
	float NdotL = max(dot(N, L), 0.0);
	vec3 H = normalize(L + V);
	float spec = pow(max(dot(N, H), 0.0), max(u_Shininess, 1.0))
		* u_SpecularStrength;

	color += albedo * u_LightColor * NdotL;
	color += u_LightColor * spec;

	// Point light with inverse-square-ish attenuation.
	vec3 toLight = u_PointLightPosition - v_WorldPosition;
	float distance = length(toLight);
	vec3 Lp = toLight / max(distance, 0.0001);

	float attenuation = 1.0 / (1.0 + 0.09 * distance
		+ 0.032 * distance * distance);

	float NdotLp = max(dot(N, Lp), 0.0);
	vec3 Hp = normalize(Lp + V);
	float specP = pow(max(dot(N, Hp), 0.0), max(u_Shininess, 1.0))
		* u_SpecularStrength;

	color += albedo * u_PointLightColor * NdotLp * attenuation;
	color += u_PointLightColor * specP * attenuation;

	FragColor = vec4(color, 1.0);
}
