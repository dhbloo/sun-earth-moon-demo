#include "RenderPass.h"

#include "Mesh.h"

RenderPass::RenderPass(PMesh mesh, PShaderProgram sp)
    : mesh(mesh)
    , shaderProg(sp)
    , culling(true)
    , pointSizeMode(false)
    , lineAntialiasing(true)
    , depthTest(true)
    , blending(false)
    , needNormalMatrix(false)
    , cullingFace(GL_BACK)
    , frontFace(GL_CCW)
    , polygonMode(GL_FILL)
    , depthFunc(GL_LESS)
    , blendSrc(GL_ONE)
    , blendDst(GL_ZERO)
    , pointSize(1.0f)
    , lineWidth(1.0f)
{
    assert(mesh);
    assert(sp);
}

Mesh &RenderPass::Mesh()
{
    return *mesh;
}

ShaderProgram &RenderPass::ShaderProg()
{
    return *shaderProg;
}

ShaderProgram::UniformSelection RenderPass::ShaderUniform(const std::string &name)
{
    return ShaderProg()[name];
}

void RenderPass::ResetMesh(PMesh m)
{
    mesh = m;
}

void RenderPass::ResetShaderProg(PShaderProgram sp)
{
    shaderProg = sp;
}

void RenderPass::SetCulling(bool enable, GLenum face, GLenum front)
{
    culling     = enable;
    cullingFace = face;
    frontFace   = front;
}

void RenderPass::SetPolygonMode(GLenum mode)
{
    polygonMode = mode;
}

void RenderPass::SetPointAttribute(bool sizeFromShader, float size)
{
    pointSizeMode = sizeFromShader;
    pointSize     = size;
}

void RenderPass::SetLineAttribute(float width, bool antialiasing)
{
    lineWidth        = width;
    lineAntialiasing = antialiasing;
}

void RenderPass::SetDepthTest(bool enable, GLenum dFunc)
{
    depthTest = enable;
    depthFunc = dFunc;
}

void RenderPass::SetBlend(bool enable, GLenum sfactor, GLenum dfactor)
{
    blending = enable;
    blendSrc = sfactor;
    blendDst = dfactor;
}

void RenderPass::SetNeedNormalMatrix(bool enable)
{
    needNormalMatrix = enable;
}

void RenderPass::Render(glm::mat4 *model, glm::mat4 *view, glm::mat4 *proj)
{
    assert(shaderProg);

    // Set shader uniform about transfrom matrix
    shaderProg->Use();
    if (model)
        ShaderUniform("model") = *model;
    if (view)
        ShaderUniform("view") = *view;
    if (proj)
        ShaderUniform("projection") = *proj;
    if (needNormalMatrix && model && view)
        ShaderUniform("normalMatrix") = glm::mat3(glm::transpose(glm::inverse(*view * *model)));

    // Set culling mode
    if (culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(cullingFace);
        glFrontFace(frontFace);
    }
    else {
        glDisable(GL_CULL_FACE);
    }

    // Set depth test
    if (depthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(depthFunc);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }

    // Set Blend Mode
    if (blending) {
        glEnable(GL_BLEND);
        glBlendFunc(blendSrc, blendDst);
    }
    else {
        glDisable(GL_BLEND);
    }

    // Set polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    if (polygonMode == GL_POINT) {
        glPointSize(pointSize);
        (pointSizeMode ? glEnable : glDisable)(GL_PROGRAM_POINT_SIZE);
    }
    else if (polygonMode == GL_LINE) {
        glLineWidth(lineWidth);
        if (lineAntialiasing) {
            // Override blend mode
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        }
        else {
            glDisable(GL_LINE_SMOOTH);
        }
    }

    mesh->Render();
}
