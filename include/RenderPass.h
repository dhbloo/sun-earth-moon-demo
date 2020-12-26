#pragma once

#include "Fwd.h"
#include "ShaderProgram.h"

class RenderPass
{
public:
    RenderPass(PMesh mesh, PShaderProgram sp);

    Mesh &                          Mesh();
    ShaderProgram &                 ShaderProg();
    ShaderProgram::UniformSelection ShaderUniform(const std::string &name);

    void ResetMesh(PMesh mesh);
    void ResetShaderProg(PShaderProgram sp);
    void SetCulling(bool enable, GLenum face = GL_BACK, GLenum front = GL_CCW);
    void SetPolygonMode(GLenum mode);
    void SetPointAttribute(bool sizeFromShader, float size = 1.0f);
    void SetLineAttribute(float width, bool antialiasing = true);
    void SetDepthTest(bool enable, GLenum depthFunc = GL_LESS);
    void SetBlend(bool enable, GLenum sfactor = GL_ONE, GLenum dfactor = GL_ZERO);
    void SetNeedNormalMatrix(bool enable);

    void Render(glm::mat4 *model, glm::mat4 *view, glm::mat4 *proj);

private:
    PMesh          mesh;
    PShaderProgram shaderProg;

    bool culling;
    bool pointSizeMode;
    bool lineAntialiasing;
    bool depthTest;
    bool blending;
    bool needNormalMatrix;

    GLenum cullingFace, frontFace;
    GLenum polygonMode;
    GLenum depthFunc;
    GLenum blendSrc, blendDst;
    float  pointSize;
    float  lineWidth;
};