#ifndef VKE_APPLICATION
#define VKE_APPLICATION

#include "Core/VKContext.hpp"
#include "Core/Window.hpp"
#include <core.hpp>

namespace VKE {
class Application {
public:
    static void init(const std::string& name, u32 width, u32 height);
    static Ref<Application> getApp();

    static void run();

    static Window& getWindow() { return *getApp()->m_Window; }
    static VKContext& getVulkanContext() { return *getApp()->m_Context; };

    static void stopEngine() { getApp()->m_Running = false; }

    Application();

private:
    Ref<VKContext> m_Context = nullptr;
    Ref<Window> m_Window = nullptr;

    static Ref<Application> m_Instance;

    bool m_Running = true;
};

}

#endif
