#pragma once

#include "Fwd.h"

template <int Type>
class Shader
{
public:
    Shader(const std::string &source);
    ~Shader() { glDeleteShader(shaderId); }

    bool        Status() const { return compileStatus; }
    std::string GetInfoLog() const;

    static PShader<Type> FromFile(const std::string &filePath);

private:
    friend class ShaderProgram;

    uint32_t shaderId;
    int      compileStatus;

    std::array<char, 1024> infoLog;
};

template <int Type>
using PShader = std::shared_ptr<Shader<Type>>;

template <int Type>
inline Shader<Type>::Shader(const std::string &source) : infoLog {}
{
    shaderId = glCreateShader(Type);

    const char *sourcePtr = source.c_str();
    glShaderSource(shaderId, 1, &sourcePtr, nullptr);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
        glGetShaderInfoLog(shaderId, (GLsizei)infoLog.size(), nullptr, infoLog.data());
        std::cerr << "Shader Compile Error: \n" << infoLog.data() << std::endl;
    }
}

template <int Type>
inline std::string Shader<Type>::GetInfoLog() const
{
    return std::string {infoLog.data()};
}

template <int Type>
inline PShader<Type> Shader<Type>::FromFile(const std::string &filePath)
{
    try {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(filePath);
        std::stringstream ss;
        ss << file.rdbuf();
        return New<Shader<Type>>(ss.str());
    }
    catch (std::exception e) {
        std::cerr << "Shader file doesn't exist: " << filePath << std::endl;
        return nullptr;
    }
}