#pragma once

#include "Fwd.h"

class ShaderProgram
{
public:
    class UniformSelection
    {
        friend class ShaderProgram;
        ShaderProgram *sp;
        int            location;

    public:
        void     operator=(bool value);
        void     operator=(int value);
        void     operator=(float value);
        void     operator=(const glm::vec2 &value);
        void     operator=(const glm::vec3 &value);
        void     operator=(const glm::vec4 &value);
        void     operator=(const glm::mat3 &value);
        void     operator=(const glm::mat4 &value);
        void     operator=(PTexture tex);
        void     operator=(PCubeMap cubeMap);
        explicit operator bool() const;
        explicit operator int() const;
        explicit operator float() const;
        explicit operator glm::vec2() const;
        explicit operator glm::vec3() const;
        explicit operator glm::vec4() const;
    };

    ShaderProgram(PVertShader vert, PFragShader frag, PGeometryShader geometry = nullptr);

    void             Use();
    UniformSelection operator[](const std::string &name);

private:
    uint32_t                          programId;
    int                               linkStatus;
    std::array<char, 1024>            infoLog;
    std::unordered_map<int, int>      locToTexIdxMap;
    std::unordered_map<int, PTexture> textureMap;
    PCubeMap                          cubeMap;
};