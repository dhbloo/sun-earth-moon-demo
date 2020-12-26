#version 330 core
in vec3 vPos;       // view space position
in vec2 vTexCoord;
in vec3 vSunPos;    // view space sun position
in mat3 vTBN;       // view space TBN matrix

uniform sampler2D diffuseTex;
uniform sampler2D normalMapTex;

uniform vec3 ambient;
uniform vec3 sunColor;

out vec4 FragColor;

void main()
{
    vec3 diffuse = texture(diffuseTex, vTexCoord).rgb;

    vec3 lightDir = vSunPos - vPos;
    float lightDist = length(lightDir);

    vec3 normal = texture(normalMapTex, vTexCoord).rgb;
    normal = normalize(2.0 * normal - 1.0); // Map [0,1] to [-1,1]
    vec3 N = normalize(vTBN * normal);
    vec3 L = normalize(lightDir);
    float lambert = max(dot(N, L), 0.0);

    vec3 fresnel = vec3(0.0);
    if (lambert > 0.0) {
        vec3 V = normalize(-vPos);
        vec3 H = normalize(V + L);
    }

    vec3 color = diffuse * ambient
               + sunColor * (diffuse * lambert) / (lightDist * lightDist);

    FragColor = vec4(color, 1.0);
} 