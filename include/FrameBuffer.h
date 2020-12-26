#pragma once

#include "Fwd.h"

class FrameBuffer
{
public:
    enum UsageType {
        R8,
        R16,
        R16F,
        R32F,

        RGB8,
        SRGB8,
        RGB16,
        RGB16F,
        RGB32F,

        RGBA8,
        SRGBA8,
        RGBA16,
        RGBA16F,
        RGBA32F,

        Depth16,
        Depth24,
        Depth32,
        Depth32F,

        Stencil8,
        Stencil16,

        Depth24_Stencil8,
        Depth32F_Stencil8
    };

    FrameBuffer(int width, int height);
    ~FrameBuffer();

    PTexture AddTextureAttachment(UsageType type);
    void     AddRenderObjAttachment(UsageType type);
    void     Use();

    static void Finish();

private:
    int      width, height;
    uint32_t fbo;
    int      colorAttachedCnt;

    std::vector<PTexture> textures;
    std::vector<uint32_t> renderObjIds;

    GLenum GetAttachmentId(UsageType type);
    GLenum GetAttachmentFormat(UsageType type);
    GLenum GetAttachmentInternalFormat(UsageType type);
};