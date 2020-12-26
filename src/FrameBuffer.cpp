#include "FrameBuffer.h"

#include "Texture.h"

FrameBuffer::FrameBuffer(int width, int height) : width(width), height(height), colorAttachedCnt(0)
{
    glGenFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &fbo);
    for (auto &id : renderObjIds)
        glDeleteRenderbuffers(1, &id);
}

GLenum FrameBuffer::GetAttachmentId(UsageType type)
{
    switch (type) {
    case Depth16:
    case Depth24:
    case Depth32:
    case Depth32F: return GL_DEPTH_ATTACHMENT;
    case Stencil8: return GL_STENCIL_ATTACHMENT;
    case Depth24_Stencil8:
    case Depth32F_Stencil8: return GL_DEPTH_STENCIL_ATTACHMENT;
    default: return GL_COLOR_ATTACHMENT0 + colorAttachedCnt++;
    }
}

GLenum FrameBuffer::GetAttachmentFormat(UsageType type)
{
    switch (type) {
    case R8:
    case R16:
    case R16F:
    case R32F: return GL_RED;
    case RGB8:
    case SRGB8:
    case RGB16:
    case RGB16F:
    case RGB32F: return GL_RGB;
    case RGBA8:
    case SRGBA8:
    case RGBA16:
    case RGBA16F:
    case RGBA32F: return GL_RGBA;
    case Depth16:
    case Depth24:
    case Depth32:
    case Depth32F: return GL_DEPTH_COMPONENT;
    case Stencil8:
    case Stencil16: return GL_STENCIL_INDEX;
    case Depth24_Stencil8:
    case Depth32F_Stencil8: return GL_DEPTH_STENCIL;
    default: assert(false);
    }
}

GLenum FrameBuffer::GetAttachmentInternalFormat(UsageType type)
{
    switch (type) {
    case R8: return GL_R8;
    case R16: return GL_R16;
    case R16F: return GL_R16F;
    case R32F: return GL_R32F;
    case RGB8: return GL_RGB8;
    case SRGB8: return GL_SRGB8;
    case RGB16: return GL_RGB16;
    case RGB16F: return GL_RGB16F;
    case RGB32F: return GL_RGB32F;
    case RGBA8: return GL_RGBA8;
    case SRGBA8: return GL_SRGB8_ALPHA8;
    case RGBA16: return GL_RGBA16;
    case RGBA16F: return GL_RGBA16F;
    case RGBA32F: return GL_RGBA32F;
    case Depth16: return GL_DEPTH_COMPONENT16;
    case Depth24: return GL_DEPTH_COMPONENT24;
    case Depth32: return GL_DEPTH_COMPONENT32;
    case Depth32F: return GL_DEPTH_COMPONENT32F;
    case Stencil8: return GL_STENCIL_INDEX8;
    case Stencil16: return GL_STENCIL_INDEX16;
    case Depth24_Stencil8: return GL_DEPTH24_STENCIL8;
    case Depth32F_Stencil8: return GL_DEPTH32F_STENCIL8;
    default: assert(false);
    }
}

PTexture FrameBuffer::AddTextureAttachment(UsageType type)
{
    GLenum   format         = GetAttachmentFormat(type);
    GLenum   internalFormat = GetAttachmentInternalFormat(type);
    PTexture tex            = New<Texture>(width, height, format, internalFormat);
    textures.push_back(tex);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GetAttachmentId(type), GL_TEXTURE_2D, tex->Id(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return tex;
}

void FrameBuffer::AddRenderObjAttachment(UsageType type)
{
    uint32_t rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GetAttachmentInternalFormat(type), width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    renderObjIds.push_back(rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GetAttachmentId(type), GL_RENDERBUFFER, rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }
}

void FrameBuffer::Finish()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
