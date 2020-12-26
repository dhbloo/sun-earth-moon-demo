#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 coord;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;  // determinant of normal matrix is one

out vec2 vTexCoord;
out vec3 vPos;      // view space position
out vec3 vNormal;   // view space normal

void main()
{
    vec4 vPos4 = view * model * vec4(pos, 1.0);

    vTexCoord = coord;
    vPos = vec3(vPos4) / vPos4.w;
    vNormal = normalize(normalMatrix * normal);

    gl_Position = projection * vPos4;
}