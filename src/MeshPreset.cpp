#include "MeshPreset.h"

#include "Mesh.h"

PMesh MeshPreset::CreateUnitBox()
{
    PMesh       mesh   = New<Mesh>();
    const float vert[] = {
        -0.5f, 0.5f,  -0.5f, 1,  1,     0,     0,     -1, 0.5f,  -0.5f, -0.5f, 0,
        0,     0,     0,     -1, -0.5f, -0.5f, -0.5f, 1,  0,     0,     0,     -1,

        0.5f,  0.5f,  -0.5f, 0,  1,     0,     0,     -1, 0.5f,  -0.5f, -0.5f, 0,
        0,     0,     0,     -1, -0.5f, 0.5f,  -0.5f, 1,  1,     0,     0,     -1,

        -0.5f, -0.5f, -0.5f, 0,  0,     -1,    0,     0,  -0.5f, -0.5f, 0.5f,  1,
        0,     -1,    0,     0,  -0.5f, 0.5f,  -0.5f, 0,  1,     -1,    0,     0,

        -0.5f, 0.5f,  0.5f,  1,  1,     -1,    0,     0,  -0.5f, 0.5f,  -0.5f, 0,
        1,     -1,    0,     0,  -0.5f, -0.5f, 0.5f,  1,  0,     -1,    0,     0,

        0.5f,  -0.5f, 0.5f,  0,  0,     1,     0,     0,  0.5f,  -0.5f, -0.5f, 1,
        0,     1,     0,     0,  0.5f,  0.5f,  0.5f,  0,  1,     1,     0,     0,

        0.5f,  0.5f,  -0.5f, 1,  1,     1,     0,     0,  0.5f,  0.5f,  0.5f,  0,
        1,     1,     0,     0,  0.5f,  -0.5f, -0.5f, 1,  0,     1,     0,     0,

        -0.5f, 0.5f,  0.5f,  0,  1,     0,     0,     1,  -0.5f, -0.5f, 0.5f,  0,
        0,     0,     0,     1,  0.5f,  0.5f,  0.5f,  1,  1,     0,     0,     1,

        0.5f,  -0.5f, 0.5f,  1,  0,     0,     0,     1,  0.5f,  0.5f,  0.5f,  1,
        1,     0,     0,     1,  -0.5f, -0.5f, 0.5f,  0,  0,     0,     0,     1,

        0.5f,  0.5f,  -0.5f, 1,  1,     0,     1,     0,  -0.5f, 0.5f,  -0.5f, 0,
        1,     0,     1,     0,  0.5f,  0.5f,  0.5f,  1,  0,     0,     1,     0,

        -0.5f, 0.5f,  0.5f,  0,  0,     0,     1,     0,  0.5f,  0.5f,  0.5f,  1,
        0,     0,     1,     0,  -0.5f, 0.5f,  -0.5f, 0,  1,     0,     1,     0,

        -0.5f, -0.5f, 0.5f,  0,  1,     0,     -1,    0,  -0.5f, -0.5f, -0.5f, 0,
        0,     0,     -1,    0,  0.5f,  -0.5f, -0.5f, 1,  0,     0,     -1,    0,

        0.5f,  -0.5f, -0.5f, 1,  0,     0,     -1,    0,  0.5f,  -0.5f, 0.5f,  1,
        1,     0,     -1,    0,  -0.5f, -0.5f, 0.5f,  0,  1,     0,     -1,    0,
    };
    mesh->SendVertexBuffer(vert, sizeof(vert) / sizeof(float), 8);
    mesh->AddVertexAttribute(0, 3, 0);
    mesh->AddVertexAttribute(1, 2, 3);
    mesh->AddVertexAttribute(2, 3, 5);

    return mesh;
}

PMesh MeshPreset::CreateUnitShpere(uint32_t level)
{
    PMesh mesh = New<Mesh>();

    std::vector<uint32_t>  indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> tangents;

    const uint32_t stacks = 1U << level, sectors = 2U << level;
    const size_t   vertCount = (stacks + 1ULL) * (sectors + 1ULL);

    positions.reserve(vertCount);
    uvs.reserve(vertCount);
    tangents.resize(vertCount);
    indices.reserve(6ULL * stacks * sectors - 12ULL * stacks);

    for (uint32_t j = 0; j <= stacks; j++) {
        float v      = (float)j / stacks;
        float phi    = PI * v;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        for (uint32_t i = 0; i <= sectors; i++) {
            float u     = (float)i / sectors;
            float theta = 2 * PI * u;

            positions.emplace_back(sinPhi * cos(theta), cosPhi, sinPhi * sin(theta));
            uvs.emplace_back(u, v);
        }
    }

    assert(vertCount == positions.size());

    for (uint32_t j = 0; j < stacks; j++) {
        uint32_t k1 = j * (sectors + 1);
        uint32_t k2 = k1 + sectors + 1;

        for (uint32_t i = 0; i < sectors; i++, k1++, k2++) {
            if (j != 0) {
                indices.push_back(k1);
                indices.push_back(k1 + 1);
                indices.push_back(k2);
            }

            if (j != stacks - 1) {
                indices.push_back(k2);
                indices.push_back(k1 + 1);
                indices.push_back(k2 + 1);
            }
        }
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 e1   = positions[indices[i + 1]] - positions[indices[i]];
        glm::vec3 e2   = positions[indices[i + 2]] - positions[indices[i]];
        glm::vec2 duv1 = uvs[indices[i + 1]] - uvs[indices[i]];
        glm::vec2 duv2 = uvs[indices[i + 2]] - uvs[indices[i]];

        glm::mat2x3 matE   = {e1, e2};
        glm::mat2x2 matDUV = {duv1, duv2};
        glm::mat2x3 matTB  = matE * glm::inverse(matDUV);
        glm::vec3   T      = glm::normalize(matTB[0]);
        glm::vec3   B      = glm::normalize(matTB[1]);

        tangents[indices[i + 2]] = tangents[indices[i + 1]] = tangents[indices[i]] = T;
    }

    std::vector<float> vert;
    vert.reserve(8 * positions.size());

    // Flatten glm::vec3 to float array and normal
    for (size_t i = 0; i < positions.size(); i++) {
        vert.push_back(positions[i].x);
        vert.push_back(positions[i].y);
        vert.push_back(positions[i].z);
        vert.push_back(uvs[i].x);
        vert.push_back(uvs[i].y);
        vert.push_back(tangents[i].x);
        vert.push_back(tangents[i].y);
        vert.push_back(tangents[i].z);
    }

    mesh->SendVertexBuffer(vert.data(), vert.size(), 8);
    mesh->AddVertexAttribute(0, 3, 0);  // position
    mesh->AddVertexAttribute(1, 2, 3);  // uv
    mesh->AddVertexAttribute(2, 3, 0);  // normal
    mesh->AddVertexAttribute(3, 3, 5);  // tangent
    mesh->SendIndexBuffer(indices.data(), indices.size());

    return mesh;
}

PMesh MeshPreset::CreateQuad()
{
    PMesh       mesh   = New<Mesh>();
    const float vert[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,
    };
    const uint32_t indices[] = {0, 1, 2, 2, 1, 3};
    mesh->SendVertexBuffer(vert, sizeof(vert) / sizeof(float), 5);
    mesh->SendIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t));
    mesh->AddVertexAttribute(0, 3, 0);
    mesh->AddVertexAttribute(1, 2, 3);

    return mesh;
}
