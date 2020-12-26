#pragma once

#include <glad/glad.h>
// first glad, then glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

class GLFWWindowBase
{
public:
    template <typename WindowType = GLFWWindowBase>
    static std::unique_ptr<WindowType> Create(int width, int height, const char *title);

    ~GLFWWindowBase();
    int          GetWidth() const;
    int          GetHeight() const;
    bool         GetKey(int key) const;
    bool         GetMouseButton(int button) const;
    glm::vec2 GetCursorPos() const;
    int          GetAverageFps() const;

    void SetTitle(const char *title);
    void SetSize(int width, int height);
    void SetFullscreen(bool enable);
    void SetVsync(bool enable);
    void SetResizable(bool enable, bool keepAspectRatio);
    void SetCursorCapture(bool enable);

    void Close();
    bool TickFrame();

protected:
    GLFWWindowBase(GLFWwindow *window);

    virtual void KeyCallback(int key, int scancode, int action, int mods);
    virtual void CursorPosCallback(glm::vec2 pos);
    virtual void CursorEnterCallback(bool entered);
    virtual void MouseButtonCallback(int button, int action, int mods);
    virtual void ScrollCallback(glm::vec2 offset);
    virtual void FrameSizeCallback(int width, int height);

    virtual void Setup()               = 0;
    virtual void Render(float elapsed) = 0;

private:
    static bool Init();

    GLFWwindow *window_;
    double      initTime_;
    double      lastTime_;
    int         averageFps_;
    int         fpsCounter_;
    bool        vsync_;
    bool        setup_;
};

template <typename WindowType>
inline std::unique_ptr<WindowType> GLFWWindowBase::Create(int width, int height, const char *title)
{
    if (!Init()) {
        return nullptr;
    }

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return nullptr;
    }

    return std::make_unique<WindowType>(window);
}
