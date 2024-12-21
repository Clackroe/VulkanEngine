#include "Core/Application.hpp"
#include <core.hpp>

#include <Core/Window.hpp>

int main()
{
    VKE::Log::Init();

    VKE::Application::init("TestApp", 1280, 720);
    VKE::Ref<VKE::Application> app = VKE::Application::getApp();
    app->run();

    VKE_INFO("Hello, World");
}
