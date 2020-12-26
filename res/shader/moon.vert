#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 coord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;  // determinant of normal matrix is one
uniform vec3 sunPos;        // world space

out vec3 vPos;      // view space position
out vec2 vTexCoord;
out vec3 vSunPos;   // view space sun position
out mat3 vTBN;      // view space TBN matrix

void main()
{
    mat4 modelView = view * model;
    vec4 vPos4 = modelView * vec4(pos, 1.0);
    vPos = vec3(vPos4) / vPos4.w;

    vec4 sunPos4 = view * vec4(sunPos, 1.0);
    vSunPos = vec3(sunPos4) / sunPos4.w;

    vTexCoord = coord;

    vec3 T = normalize(mat3(modelView) * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);   // re-orthogonalize T with respect to N
    vec3 B = cross(N, T);

    vTBN = mat3(T, B, N);

    gl_Position = projection * vPos4;
}