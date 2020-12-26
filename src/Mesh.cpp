#include "Mesh.h"

Mesh::Mesh() : vertCount(0), indexCount(0), stride(0)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::SendIndexBuffer(const uint32_t *indexs, size_t count)
{
    indexCount = count;

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(count * sizeof(uint32_t)),
                 indexs,
                 GL_STATIC_DRAW);
}

void Mesh::AddVertexAttribute(uint32_t idx, int size, int offset, int elementSize)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(idx,
                          size,
                          GL_FLOAT,
                          false,
                          stride * elementSize,
                          (void *)((intptr_t)offset * (intptr_t)elementSize));
    glEnableVertexAttribArray(idx);
}

void Mesh::Render()
{
    glBindVertexArray(vao);

    // Draw call
    if (indexCount) {
        glDrawElements(GL_TRIANGLES, (GLsizei)indexCount, GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertCount);
    }
}

size_t Mesh::VertexCount() const
{
    return vertCount;
}

size_t Mesh::TriangleCount() const
{
    return indexCount ? indexCount / 3 : vertCount / 3;
}

void Mesh::SendVertexBufferImpl(const void *data, size_t elemSize, size_t count, int vertexStride)
{
    stride    = vertexStride;
    vertCount = count / stride;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(count * elemSize), data, GL_STATIC_DRAW);
}
