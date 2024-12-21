
#include <Core/Window.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VKE {
Window::Window(const std::string& name, u32 width, u32 height)
    : m_Width(width)
    , m_Height(height)
    , m_Name(name)
{
    int success = glfwInit();
    VKE_ASSERT(success, "Failed to init the GLFW Window");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);

    VKE_INFO("Window Initialized")
}

void Window::pollEnvents()
{
    glfwPollEvents();
}

}
