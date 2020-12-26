#pragma once

#include "Fwd.h"

class Mesh
{
public:
    Mesh();
    ~Mesh();

    template <typename T>
    void SendVertexBuffer(const T *data, size_t count, int stride);
    void SendIndexBuffer(const uint32_t *indexs, size_t count);
    void AddVertexAttribute(uint32_t idx, int size, int offset, int elementSize = sizeof(float));

    void Render();

    size_t VertexCount() const;
    size_t TriangleCount() const;

private:
    void SendVertexBufferImpl(const void *data, size_t elemSize, size_t count, int stride);

    uint32_t vbo, vao, ebo;
    int      stride;
    size_t   vertCount, vertexSize, indexCount;
};

template <typename T>
void Mesh::SendVertexBuffer(const T *data, size_t count, int vertexStride)
{
    SendVertexBufferImpl(data, sizeof(T), count, vertexStride);
}