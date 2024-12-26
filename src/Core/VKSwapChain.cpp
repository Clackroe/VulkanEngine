
#include "Core/Application.hpp"
#include "GLFW/glfw3.h"
#include <Core/VKSwapChain.hpp>
#include <vulkan/vulkan_core.h>

namespace VKE {

VulkanSwapchain::VulkanSwapchain(const VkInstance& instance)
{
    Window& window = Application::getWindow();
    VkResult res = glfwCreateWindowSurface(instance, window.getGLFW(), nullptr, &m_Surface);
    VKE_ASSERT(res == VK_SUCCESS, "Failed to create glfw surface")
    VKE_INFO("Created Vulkan/GLFW Surface")

    VKContext::pushCleanupFunc([=]() { vkDestroySurfaceKHR(instance, m_Surface, nullptr); });

    VulkanPhysicalDevice& pDevice = Application::getVulkanContext().getPhysicalDevice();

    std::vector<VkQueueFamilyProperties> queueProps = pDevice.getQueueFamilyProps();

    std::vector<VkBool32> supportsPresent(queueProps.size());
    for (u32 i = 0; i < queueProps.size(); i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(pDevice.getVulkanPhysical(), i, m_Surface, &supportsPresent[i]);
    }

    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueProps.size(); i++) {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    if (presentQueueNodeIndex == UINT32_MAX) {
        for (uint32_t i = 0; i < queueProps.size(); ++i) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    VKE_ASSERT(graphicsQueueNodeIndex != UINT32_MAX);
    VKE_ASSERT(presentQueueNodeIndex != UINT32_MAX);

    pDevice.setPresentQueue(presentQueueNodeIndex);
}

VulkanSwapchain::~VulkanSwapchain()
{
}
}
