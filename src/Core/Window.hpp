#ifndef VKE_WINDOW
#define VKE_WINDOW

#include "GLFW/glfw3.h"
#include <core.hpp>

class GLFWwindow;

namespace VKE {

class Window {

public:
    Window(const std::string& name, u32 width, u32 height);
    ~Window()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    };

    u32 getWidth() { return m_Height; }
    u32 getHeight() { return m_Width; }

    const std::string& getName() const { return m_Name; }

    void pollEnvents();

    GLFWwindow* getGLFW() { return m_Window; };

private:
    u32 m_Width, m_Height;

    std::string m_Name;

    GLFWwindow* m_Window;
};

}

#endif
