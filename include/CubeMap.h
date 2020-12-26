#pragma once

#include "Fwd.h"

class CubeMap
{
public:
    CubeMap(const std::array<std::string, 6> &path, bool gammaCorrection = true);
    ~CubeMap();

    void SetProperty(GLenum pname, int param);
    void Setup();
    void Use();

private:
    uint32_t                        cubeMapId;
    GLenum                          internalFormat; 
    bool                            setup;
    std::array<std::string, 6>      facePaths;
    std::unordered_map<GLenum, int> properties;
};

using PCubeMap = std::shared_ptr<CubeMap>;
