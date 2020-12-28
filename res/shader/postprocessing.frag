#version 330 core
in vec2 texCoord;

uniform sampler2D inputTexture;
uniform sampler2D bloomTexture;
uniform float exposure;
uniform float bloom;

out vec4 FragColor;

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
	const float A = 2.51;
	const float B = 0.03;
	const float C = 2.43;
	const float D = 0.59;
	const float E = 0.14;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main()
{
    vec3 color = texture(inputTexture, texCoord).rgb;
    color += texture(bloomTexture, texCoord).rgb * bloom;

    // AECS tone mapping
    vec3 mapped = ACESToneMapping(color, exposure);

    FragColor = vec4(mapped, 1.0);
}