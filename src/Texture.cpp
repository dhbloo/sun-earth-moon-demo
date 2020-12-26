#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string &path, bool gammaCorrection) : setupOK(false)
{
    glGenTextures(1, &texId);

    int nChannels;
    stbi_set_flip_vertically_on_load(false);
    uint8_t *data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
    if (!data) {
        std::cerr << "Unable to load texture: " << path << std::endl;
        return;
    }

    switch (nChannels) {
    case 4:
        internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
        dataFormat     = GL_RGBA;
        break;
    case 3:
        internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
        dataFormat     = GL_RGB;
        break;
    case 1:
        internalFormat = GL_RED;
        dataFormat     = GL_RED;
        break;
    default: assert(("unsupported texture format!", false));
    }

    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormat,
                 width,
                 height,
                 0,
                 dataFormat,
                 GL_UNSIGNED_BYTE,
                 data);

    stbi_image_free(data);

    // default properties
    properties[GL_TEXTURE_MIN_FILTER] = GL_LINEAR_MIPMAP_LINEAR;
    properties[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
}

Texture::Texture(int width, int height, GLenum format, GLenum internalFormat)
    : width(width)
    , height(height)
    , dataFormat(format)
    , internalFormat(internalFormat)
    , setupOK(false)
{
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormat,
                 width,
                 height,
                 0,
                 dataFormat,
                 GL_UNSIGNED_BYTE,
                 nullptr);

    // default properties
    properties[GL_TEXTURE_MIN_FILTER] = GL_LINEAR_MIPMAP_LINEAR;
    properties[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
}

Texture::~Texture()
{
    glDeleteTextures(1, &texId);
}

void Texture::SetProperty(GLenum pname, int param)
{
    properties[pname] = param;
}

void Texture::Setup(bool mipmap, bool expendChannels)
{
    glBindTexture(GL_TEXTURE_2D, texId);

    if (expendChannels && internalFormat == GL_RED) {
        GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
    }

    for (auto &p : properties) {
        glTexParameteri(GL_TEXTURE_2D, p.first, p.second);
    }

    if (mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else if (properties[GL_TEXTURE_MIN_FILTER] != GL_NEAREST
             && properties[GL_TEXTURE_MIN_FILTER] != GL_LINEAR) {
        // Set tex parameter to GL_LINEAR when mipmap is disabled
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    setupOK = true;
}

void Texture::Use(uint32_t texUnit)
{
    if (!setupOK)
        Setup();
    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, texId);
}
