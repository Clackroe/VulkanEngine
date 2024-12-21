#include "Core/Input.hpp"
#include <Core/Application.hpp>

namespace VKE {

Ref<Application> Application::m_Instance = nullptr;

Application::Application(const std::string& name, u32 width, u32 height)
    : m_Window(name, width, height)
    , m_Context(name)
{
}

void Application::run()
{
    auto app = getApp();
    while (app->m_Running) {
        app->m_Window.pollEnvents();
        if (Input::isKeyDown(Key::Escape)) {
            stopEngine();
        }
    }
}

void Application::init(const std::string& name, u32 width, u32 height)
{
    if (!m_Instance) {
        m_Instance = CreateRef<Application>(name, width, height);
        Input::Init();
    }
}
Ref<Application> Application::getApp()
{
    if (m_Instance) {
        return m_Instance;
    }
    VKE_ERROR("Tried to get null application instance");
    return nullptr;
}

}
