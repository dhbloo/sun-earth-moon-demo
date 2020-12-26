#include "GLFWWindow.h"

#include <cassert>
#include <iostream>
#include <mutex>

GLFWWindowBase::~GLFWWindowBase()
{
    glfwDestroyWindow(window_);
}

GLFWWindowBase::GLFWWindowBase(GLFWwindow *window)
    : window_(window)
    , averageFps_(0)
    , vsync_(false)
    , fpsCounter_(0)
    , setup_(false)
    , initTime_()
    , lastTime_()
{
    assert(window_);
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);
    glfwSwapInterval(vsync_);

    glfwSetKeyCallback(window_, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        GLFWWindowBase *pWindow = static_cast<GLFWWindowBase *>(glfwGetWindowUserPointer(w));
        pWindow->KeyCallback(key, scancode, action, mods);
    });
    glfwSetCursorPosCallback(window_, [](GLFWwindow *w, double xpos, double ypos) {
        GLFWWindowBase *pWindow = static_cast<GLFWWindowBase *>(glfwGetWindowUserPointer(w));
        pWindow->CursorPosCallback(glm::vec2 {xpos, ypos});
    });
    glfwSetCursorEnterCallback(window_, [](GLFWwindow *w, int entered) {
        GLFWWindowBase *pWindow = static_cast<GLFWWindowBase *>(glfwGetWindowUserPointer(w));
        pWindow->CursorEnterCallback(!!entered);
    });
    glfwSetMouseButtonCallback(window_, [](GLFWwindow *w, int button, int action, int mods) {
        GLFWWindowBase *pWindow = static_cast<GLFWWindowBase *>(glfwGetWindowUserPointer(w));
        pWindow->MouseButtonCallback(button, action, mods);
    });
    glfwSetScrollCallback(window_, [](GLFWwindow *w, double xoffset, double yoffset) {
        GLFWWindowBase *pWindow = static_cast<GLFWWindowBase *>(glfwGetWindowUserPointer(w));
        pWindow->ScrollCallback(glm::vec2 {xoffset, yoffset});
    });
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *w, int width, int height) {
        GLFWWindowBase *pWindow = static_cast<GLFWWindowBase *>(glfwGetWindowUserPointer(w));
        pWindow->FrameSizeCallback(width, height);
    });
}

int GLFWWindowBase::GetWidth() const
{
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    return width;
}

int GLFWWindowBase::GetHeight() const
{
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    return height;
}

bool GLFWWindowBase::GetKey(int key) const
{
    return glfwGetKey(window_, key) != GLFW_RELEASE;
}

bool GLFWWindowBase::GetMouseButton(int button) const
{
    return glfwGetMouseButton(window_, button) != GLFW_RELEASE;
}

glm::vec2 GLFWWindowBase::GetCursorPos() const
{
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    return glm::vec2 {x, y};
}

int GLFWWindowBase::GetAverageFps() const
{
    return averageFps_;
}

void GLFWWindowBase::SetTitle(const char *title)
{
    glfwSetWindowTitle(window_, title);
}

void GLFWWindowBase::SetSize(int width, int height)
{
    glfwSetWindowSize(window_, width, height);
}

void GLFWWindowBase::SetFullscreen(bool enable)
{
    bool isFullscreen = glfwGetWindowMonitor(window_);
    if (enable == isFullscreen)
        return;

    if (enable) {
        GLFWmonitor *      monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode    = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else {
        glfwSetWindowMonitor(window_, nullptr, 0, 0, GetWidth(), GetHeight(), GLFW_DONT_CARE);
    }
}

void GLFWWindowBase::SetVsync(bool enable)
{
    if (vsync_ != enable) {
        vsync_              = enable;
        GLFWwindow *prevCtx = glfwGetCurrentContext();
        glfwMakeContextCurrent(window_);
        glfwSwapInterval(vsync_);
        glfwMakeContextCurrent(prevCtx);
    }
}

void GLFWWindowBase::SetResizable(bool enable, bool keepAspectRatio)
{
    glfwSetWindowAttrib(window_, GLFW_RESIZABLE, enable);
    if (keepAspectRatio) {
        glfwSetWindowAspectRatio(window_, GetWidth(), GetHeight());
    }
    else {
        glfwSetWindowAspectRatio(window_, GLFW_DONT_CARE, GLFW_DONT_CARE);
    }
}

void GLFWWindowBase::SetCursorCapture(bool enable)
{
    glfwSetInputMode(window_, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GLFWWindowBase::Close()
{
    glfwSetWindowShouldClose(window_, 1);
}

bool GLFWWindowBase::TickFrame()
{
    if (glfwWindowShouldClose(window_)) {
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwPollEvents();

    if (!setup_) {
        setup_ = true;
        Setup();
        lastTime_ = initTime_ = glfwGetTime();
    }

    fpsCounter_++;
    double nowTime = glfwGetTime();
    if (nowTime - lastTime_ >= 1) {
        averageFps_ = fpsCounter_;
        lastTime_   = nowTime;
        fpsCounter_ = 0;
    }

    Render(float(nowTime - initTime_));
    glfwSwapBuffers(window_);

    return true;
}

void GLFWWindowBase::KeyCallback(int key, int scancode, int action, int mods) {}

void GLFWWindowBase::CursorPosCallback(glm::vec2 pos) {}

void GLFWWindowBase::CursorEnterCallback(bool entered) {}

void GLFWWindowBase::MouseButtonCallback(int button, int action, int mods) {}

void GLFWWindowBase::ScrollCallback(glm::vec2 offset) {}

void GLFWWindowBase::FrameSizeCallback(int width, int height) {}

bool GLFWWindowBase::Init()
{
    static struct GLFWInitializer
    {
        GLFWInitializer()
        {
            if (!(ok = glfwInit()))
                return;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

            glfwSetErrorCallback([](int error_code, const char *description) {
                std::cerr << "GLFW error(" << error_code << "): " << description << '\n';
            });
        }
        ~GLFWInitializer() { glfwTerminate(); }
        bool ok;
    } initializer;

    return initializer.ok;
}
