#include <core.hpp>

#include <GLFW/glfw3.h>

int main()
{
    GBP::Log::Init();

    glm::vec3 testVec = glm::vec3({ 69.0, 420.0, 42069 });
    GBP_INFO("Hello, World");
    GBP_INFO("    - GLFW Compiles and Links");
    GBP_INFO("    - GLM Compiles and Links");
}
