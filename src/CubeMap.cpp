#include "CubeMap.h"

#include <stb_image.h>

CubeMap::CubeMap(const std::array<std::string, 6> &path, bool gammaCorrection)
    : facePaths(path)
    , setup(false)
{
    glGenTextures(1, &cubeMapId);

    // default properties
    properties[GL_TEXTURE_MIN_FILTER] = GL_LINEAR;
    properties[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
    properties[GL_TEXTURE_WRAP_R]     = GL_CLAMP_TO_EDGE;
    properties[GL_TEXTURE_WRAP_S]     = GL_CLAMP_TO_EDGE;
    properties[GL_TEXTURE_WRAP_T]     = GL_CLAMP_TO_EDGE;

    internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
}

CubeMap::~CubeMap()
{
    glDeleteTextures(1, &cubeMapId);
}

void CubeMap::SetProperty(GLenum pname, int param)
{
    properties[pname] = param;
}

void CubeMap::Setup()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapId);

    for (auto &p : properties) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, p.first, p.second);
    }

    int width, height, nChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < 6; i++) {
        uint8_t *data = stbi_load(facePaths[i].c_str(), &width, &height, &nChannels, 3);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         internalFormat,
                         width,
                         height,
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Cubemap texture failed to load at path: " << facePaths[i] << std::endl;
        }
    }

    setup = true;
}

void CubeMap::Use()
{
    if (!setup)
        Setup();
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapId);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
