#ifndef VKE_SWAPCHAIN
#define VKE_SWAPCHAIN

#include <core.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace VKE {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct SwapchainImages {
    std::vector<VkImage> images;
    std::vector<VkImageView> views;
};

class VulkanSwapchain {
public:
    VulkanSwapchain(const VkInstance& instance);
    ~VulkanSwapchain();

private:
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    void chooseSwapSurfaceFormat();
    void chooseSwapPresentMode();
    void chooseSwapExtent();

    void initImages();

private:
    VkFormat m_ColorFormat;
    VkColorSpaceKHR m_ColorSpace;
    VkPresentModeKHR m_PresentMode;

    VkExtent2D m_SwapExtent;

    u32 m_ImageCount;

    SwapChainSupportDetails m_SwapchainSupportDetails;

    SwapchainImages m_Images;

    VkSwapchainKHR m_SwapChain;
};

}

#endif
