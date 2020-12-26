cmake_minimum_required(VERSION 3.2)
project(GLDemo)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

add_subdirectory(deps/glfw)

file(GLOB DEPS_SOURCES deps/glad/src/glad.c)
source_group("deps" FILES ${DEPS_SOURCES})

file(GLOB SOURCES 
    src/main.cpp 
    src/GLFWWindow.cpp
    src/CubeMap.cpp
    src/Mesh.cpp
    src/MeshPreset.cpp
    src/Shader.cpp
    src/ShaderProgram.cpp
    src/Texture.cpp
    src/RenderPass.cpp
    src/FrameBuffer.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${DEPS_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    deps/glfw/include
    deps/glad/include
    deps/glm
    deps/stb
)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)