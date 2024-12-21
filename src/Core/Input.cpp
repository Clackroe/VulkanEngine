#include "Core/Application.hpp"
#include "Core/Window.hpp"
#include <Core/Input.hpp>

#include <GLFW/glfw3.h>
namespace VKE {

bool Input::isKeyDown(KeyCode key)
{
    return getInstance().m_KeyStates[key];
};
bool Input::isKeyReleased(KeyCode key)
{
    return !getInstance().m_KeyStates[key];
};

float Input::getMouseX()
{
    return getInstance().m_MouseX;
}
float Input::getMouseY()
{
    return getInstance().m_MouseY;
}

bool Input::isMouseDown(KeyCode key)
{
    return getInstance().m_MouseButtonStates[key];
}
bool Input::isMouseReleased(KeyCode key)
{
    return !getInstance().m_MouseButtonStates[key];
}

//---

void Input::keyPressed(KeyCode key)
{
    m_KeyStates[key] = true;
}
void Input::keyReleased(KeyCode key)
{
    m_KeyStates[key] = false;
}
void Input::mousePressed(MouseCode key)
{
    m_MouseButtonStates[key] = true;
}
void Input::mouseReleased(MouseCode key)
{
    m_MouseButtonStates[key] = false;
}

void Input::mouseMoved(float x, float y)
{
    m_MouseX = x;
    m_MouseY = y;
}

// ----
static void mouseMovedCallback(GLFWwindow* window, double xPos, double yPos)
{

    Input::getInstance().mouseMoved(xPos, yPos);
}

static void keyCallBack(GLFWwindow* window, int key, int scanCode, int action, int mods)
{

    Mode mode = -1;
    switch (action) {
    case GLFW_REPEAT:
        mode = Key::REPEAT;
        break;
    case GLFW_PRESS:
        mode = Key::PRESS;
        Input::getInstance().keyPressed(static_cast<KeyCode>(key));
        break;
    case GLFW_RELEASE:
        mode = Key::RELEASE;
        Input::getInstance().keyReleased(static_cast<KeyCode>(key));
        break;
    default:
        VKE_WARN("UNKNOWN KEY MODE/ACTION");
        break;
    }
};

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Mode mode = -1;
    switch (action) {
    case GLFW_REPEAT:
        mode = Key::REPEAT;
        break;
    case GLFW_PRESS:
        mode = Key::PRESS;
        Input::getInstance().mousePressed(static_cast<MouseCode>(button));
        break;
    case GLFW_RELEASE:
        mode = Key::RELEASE;
        Input::getInstance().mouseReleased(static_cast<MouseCode>(button));
        break;
    default:
        VKE_WARN("UNKNOWN KEY MODE/ACTION");
        break;
    }
}

// ----

void Input::Init()
{
    auto window = Application::getApp()->getWindow().getGLFW();
    glfwSetKeyCallback(window, keyCallBack);
    glfwSetCursorPosCallback(window, mouseMovedCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    VKE_INFO("Input System Initialized: ");
}
}
