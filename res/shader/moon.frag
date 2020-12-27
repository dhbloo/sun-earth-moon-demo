#version 330 core
in vec3 vPos;       // view space position
in vec2 vTexCoord;
in vec3 vSunPos;    // view space sun position
in vec3 vEarthPos;  // view space earth position
in mat3 vTBN;       // view space TBN matrix

uniform mat4 view;
uniform sampler2D diffuseTex;
uniform sampler2D normalMapTex;

uniform vec3 ambient;
uniform vec3 sunColor;
uniform float earthRadius;

out vec4 FragColor;

float earthR = earthRadius * length(mat3(view) * vec3(1)) / length(vec3(1));

float intersectSphere(vec3 o, vec3 d, vec3 c, float r)
{
    vec3 op = c - o;
    float b = dot(op, d);
    float delta = b * b - dot(op, op) + r * r;

    if (delta < 0.0)
        return 0.0; 		        
    else
        delta = sqrt(delta);
    
    float t;
    if ((t = b - delta) > 1e-4)
        return t;
    else if ((t = b + delta) > 1e-4)
        return t;
    else
        return 0.0;
}

void main()
{
    vec3 diffuse = texture(diffuseTex, vTexCoord).rgb;

    vec3 lightDir = vSunPos - vPos;
    vec3 L = normalize(lightDir);
    float lightDist = length(lightDir);
    
    float lambert = 0.0;
    if (intersectSphere(vPos, L, vEarthPos, earthR) == 0.0) {
        vec3 normal = texture(normalMapTex, vTexCoord).rgb;
        normal = normalize(2.0 * normal - 1.0); // Map [0,1] to [-1,1]
        vec3 N = normalize(vTBN * normal);

        lambert = max(dot(N, L), 0.0);
    }

    vec3 color = diffuse * ambient
               + sunColor * (diffuse * lambert) / (lightDist * lightDist);

    FragColor = vec4(color, 1.0);
} 