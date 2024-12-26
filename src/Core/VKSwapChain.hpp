#ifndef VKE_SWAPCHAIN
#define VKE_SWAPCHAIN

#include <core.hpp>
#include <vulkan/vulkan_core.h>

namespace VKE {

class VulkanSwapchain {
public:
    VulkanSwapchain(const VkInstance& instance);
    ~VulkanSwapchain();

private:
    VkSurfaceKHR m_Surface;
};

}

#endif
