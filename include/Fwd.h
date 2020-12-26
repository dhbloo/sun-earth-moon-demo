#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

// third-party headers
#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh;
template <int Type>
class Shader;
using VertShader     = Shader<GL_VERTEX_SHADER>;
using FragShader     = Shader<GL_FRAGMENT_SHADER>;
using GeometryShader = Shader<GL_GEOMETRY_SHADER>;
class ShaderProgram;
class Texture;
class CubeMap;
class RenderPass;
class FrameBuffer;

using PMesh = std::shared_ptr<Mesh>;
template <int Type>
using PShader         = std::shared_ptr<Shader<Type>>;
using PVertShader     = std::shared_ptr<VertShader>;
using PFragShader     = std::shared_ptr<FragShader>;
using PGeometryShader = std::shared_ptr<GeometryShader>;
using PShaderProgram  = std::shared_ptr<ShaderProgram>;
using PTexture        = std::shared_ptr<Texture>;
using PCubeMap        = std::shared_ptr<CubeMap>;
using PRenderPass     = std::shared_ptr<RenderPass>;
using PFrameBuffer    = std::shared_ptr<FrameBuffer>;

constexpr float PI = glm::pi<float>();

template <typename T, typename... Args>
inline auto New(Args... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}