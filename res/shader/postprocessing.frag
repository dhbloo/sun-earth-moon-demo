#version 330 core
in vec2 texCoord;

uniform sampler2D inputTexture;
uniform sampler2D bloomTexture;
uniform float exposure;
uniform float bloom;

out vec4 FragColor;

void main()
{
    vec3 color = texture(inputTexture, texCoord).rgb;
    color += texture(bloomTexture, texCoord).rgb * bloom;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * exposure);

    FragColor = vec4(mapped, 1.0);
}