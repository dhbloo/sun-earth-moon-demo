#include "ShaderProgram.h"

#include "CubeMap.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(PVertShader vert, PFragShader frag, PGeometryShader geometry)
    : cubeMap(nullptr)
{
    assert(vert && frag);

    programId = glCreateProgram();
    glAttachShader(programId, vert->shaderId);
    glAttachShader(programId, frag->shaderId);

    if (geometry)
        glAttachShader(programId, geometry->shaderId);

    glLinkProgram(programId);

    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        glGetShaderInfoLog(programId, (GLsizei)infoLog.size(), nullptr, infoLog.data());
        std::cerr << "Shader Link Error: \n" << infoLog.data() << std::endl;
    }
}

void ShaderProgram::Use()
{
    glUseProgram(programId);
    for (auto &texMap : textureMap) {
        texMap.second->Use(texMap.first);
    }
    if (cubeMap)
        cubeMap->Use();
}

ShaderProgram::UniformSelection ShaderProgram::operator[](const std::string &name)
{
    UniformSelection selection;
    selection.sp       = this;
    selection.location = glGetUniformLocation(programId, name.c_str());
    if (selection.location == -1) {
        std::cerr << "Shader uniform '" << name << "' assigned to invalid location\n";
    }
    return selection;
}

void ShaderProgram::UniformSelection::operator=(bool value)
{
    glUseProgram(sp->programId);
    glUniform1i(location, (int)value);
}

void ShaderProgram::UniformSelection::operator=(int value)
{
    glUseProgram(sp->programId);
    glUniform1i(location, value);
}

void ShaderProgram::UniformSelection::operator=(float value)
{
    glUseProgram(sp->programId);
    glUniform1f(location, value);
}

void ShaderProgram::UniformSelection::operator=(const glm::vec2 &value)
{
    glUseProgram(sp->programId);
    glUniform2f(location, value[0], value[1]);
}

void ShaderProgram::UniformSelection::operator=(const glm::vec3 &value)
{
    glUseProgram(sp->programId);
    glUniform3f(location, value[0], value[1], value[2]);
}

void ShaderProgram::UniformSelection::operator=(const glm::vec4 &value)
{
    glUseProgram(sp->programId);
    glUniform4f(location, value[0], value[1], value[2], value[3]);
}

void ShaderProgram::UniformSelection::operator=(const glm::mat3 &value)
{
    glUseProgram(sp->programId);
    glUniformMatrix3fv(location, 1, false, glm::value_ptr(value));
}

void ShaderProgram::UniformSelection::operator=(const glm::mat4 &value)
{
    glUseProgram(sp->programId);
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
}

void ShaderProgram::UniformSelection::operator=(PTexture tex)
{
    if (location == -1)
        return;

    glUseProgram(sp->programId);

    auto texIdxIt = sp->locToTexIdxMap.find(location);
    if (texIdxIt != sp->locToTexIdxMap.end()) {
        auto it = sp->textureMap.find(texIdxIt->second);
        sp->textureMap.erase(it);
        glUniform1i(location, 0);  // reset to default
    }

    if (!tex)
        return;

    int   idx = 0;
    GLint maxCombinedTextureImageUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits);
    for (; idx < maxCombinedTextureImageUnits; idx++)
        if (sp->textureMap.find(idx) == sp->textureMap.end()) {
            sp->textureMap[idx]          = tex;
            sp->locToTexIdxMap[location] = idx;
            break;
        }

    if (idx == maxCombinedTextureImageUnits)  // bind fail, texture unit exhaust
        return;

    glUniform1i(location, idx);
}

void ShaderProgram::UniformSelection::operator=(PCubeMap cubeMap)
{
    sp->cubeMap = cubeMap;
}

ShaderProgram::UniformSelection::operator bool() const
{
    int val;
    glGetUniformiv(sp->programId, location, &val);
    return val;
}

ShaderProgram::UniformSelection::operator int() const
{
    int val;
    glGetUniformiv(sp->programId, location, &val);
    return val;
}

ShaderProgram::UniformSelection::operator float() const
{
    float val;
    glGetUniformfv(sp->programId, location, &val);
    return val;
}

ShaderProgram::UniformSelection::operator glm::vec2() const
{
    glm::vec2 val;
    glGetUniformfv(sp->programId, location, &val[0]);
    return val;
}

ShaderProgram::UniformSelection::operator glm::vec3() const
{
    glm::vec3 val;
    glGetUniformfv(sp->programId, location, &val[0]);
    return val;
}

ShaderProgram::UniformSelection::operator glm::vec4() const
{
    glm::vec4 val;
    glGetUniformfv(sp->programId, location, &val[0]);
    return val;
}