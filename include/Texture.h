#pragma once

#include "Fwd.h"

class Texture
{
public:
    Texture(const std::string &texturePath, bool gammaCorrection = true);
    Texture(int width, int height, GLenum format, GLenum internalFormat);
    ~Texture();

    void     SetProperty(GLenum pname, int param);
    void     Setup(bool mipmap = true, bool expendChannels = true);
    void     Use(uint32_t texUnit = 0);
    uint32_t Id() { return texId; }

private:
    uint32_t                        texId;
    int                             width, height;
    GLenum                          internalFormat, dataFormat;
    bool                            setupOK;
    std::unordered_map<GLenum, int> properties;
};