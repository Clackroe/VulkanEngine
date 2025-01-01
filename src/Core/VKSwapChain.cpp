
#include "Core/Application.hpp"
#include "Core/VKDevice.hpp"
#include "GLFW/glfw3.h"
#include "core.hpp"
#include <Core/VKSwapChain.hpp>
#include <vulkan/vulkan_core.h>

namespace VKE {

void VulkanSwapchain::chooseSwapExtent()
{
    auto& capabilities = m_SwapchainSupportDetails.capabilities;

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        m_SwapExtent = capabilities.currentExtent;
        return;
    } else {
        int width, height;
        glfwGetFramebufferSize(Application::getApp()->getWindow().getGLFW(), &width, &height);

        VkExtent2D actualExtent = {
            (uint32_t)width,
            (uint32_t)height
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        m_SwapExtent = actualExtent;
    }
}

void VulkanSwapchain::chooseSwapPresentMode()
{
    for (const auto& availablePresentMode : m_SwapchainSupportDetails.presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            m_PresentMode = availablePresentMode;
            return;
        }
    }

    m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
}

SwapChainSupportDetails VulkanSwapchain::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Application::getVulkanContext().getSurface(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, Application::getVulkanContext().getSurface(), &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, Application::getVulkanContext().getSurface(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, Application::getVulkanContext().getSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, Application::getVulkanContext().getSurface(), &presentModeCount, details.presentModes.data());
    }

    bool swapChainAdequate = false;
    swapChainAdequate = !details.formats.empty() && !details.presentModes.empty();

    if (!swapChainAdequate) {
        VKE_ERROR("Swapchain is NOT Adequate");
    }

    return details;
}
void VulkanSwapchain::chooseSwapSurfaceFormat()
{
    VulkanPhysicalDevice& pDevice = Application::getVulkanContext().getPhysicalDevice();

    u32 formatCount = m_SwapchainSupportDetails.formats.size();
    auto& surfaceFormats = m_SwapchainSupportDetails.formats;

    // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
    // there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
    if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
        m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
        m_ColorSpace = surfaceFormats[0].colorSpace;
    } else {
        // iterate over the list of available surface format and
        // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
        bool found_B8G8R8A8_UNORM = false;
        for (auto&& surfaceFormat : surfaceFormats) {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
                m_ColorFormat = surfaceFormat.format;
                m_ColorSpace = surfaceFormat.colorSpace;
                found_B8G8R8A8_UNORM = true;
                break;
            }
        }

        // in case VK_FORMAT_B8G8R8A8_UNORM is not available
        // select the first available color format
        if (!found_B8G8R8A8_UNORM) {
            m_ColorFormat = surfaceFormats[0].format;
            m_ColorSpace = surfaceFormats[0].colorSpace;
        }
    }
}

void VulkanSwapchain::initImages()
{
    VulkanDevice device = Application::getApp()->getVulkanContext().getDevice();

    vkGetSwapchainImagesKHR(device.getVulkanDevice(), m_SwapChain, &m_ImageCount, nullptr);
    m_Images.images.resize(m_ImageCount);
    m_Images.views.resize(m_ImageCount);
    vkGetSwapchainImagesKHR(device.getVulkanDevice(), m_SwapChain, &m_ImageCount, m_Images.images.data());

    for (u32 i = 0; i < m_ImageCount; i++) {
        VkImageViewCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_Images.images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_ColorFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult res = vkCreateImageView(device.getVulkanDevice(), &createInfo, nullptr, &m_Images.views[i]);
        VKE_ASSERT(res == VK_SUCCESS, "Failed to create swapchain image: " + std::to_string(i))

        Application::getVulkanContext().pushCleanupFunc([=, this]() {
            vkDestroyImageView(device.getVulkanDevice(), m_Images.views[i], nullptr);
        });
    }
}

VulkanSwapchain::VulkanSwapchain(const VkInstance& instance)
{

    VKContext::pushCleanupFunc([=]() { vkDestroySurfaceKHR(instance, Application::getVulkanContext().getSurface(), nullptr); });

    VulkanPhysicalDevice& pDevice = Application::getVulkanContext().getPhysicalDevice();

    m_SwapchainSupportDetails = querySwapChainSupport(pDevice.getVulkanPhysical());
    chooseSwapSurfaceFormat();
    chooseSwapPresentMode();
    chooseSwapExtent();

    m_ImageCount = m_SwapchainSupportDetails.capabilities.minImageCount + 1;

    if (m_SwapchainSupportDetails.capabilities.maxImageCount > 0 && m_ImageCount > m_SwapchainSupportDetails.capabilities.maxImageCount) {
        m_ImageCount = m_SwapchainSupportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = Application::getVulkanContext().getSurface();
    createInfo.minImageCount = m_ImageCount;
    createInfo.imageFormat = m_ColorFormat;
    createInfo.imageColorSpace = m_ColorSpace;
    createInfo.imageExtent = m_SwapExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (m_SwapchainSupportDetails.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    // Enable transfer destination on swap chain images if supported
    if (m_SwapchainSupportDetails.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    auto& queueIndices = pDevice.getQueueIndices();

    u32 queueFamilyIndices[] = { queueIndices.present, queueIndices.graphics };
    bool seperatePresGraphicsQueue = queueIndices.present != queueIndices.graphics;
    if (seperatePresGraphicsQueue) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = m_SwapchainSupportDetails.capabilities.currentTransform;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_PresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult res = vkCreateSwapchainKHR(Application::getApp()->getVulkanContext().getDevice().getVulkanDevice(), &createInfo, nullptr, &m_SwapChain);
    VKE_ASSERT(res == VK_SUCCESS, "Failed to create swapchain");
    Application::getVulkanContext().pushCleanupFunc([=]() {
        vkDestroySwapchainKHR(Application::getApp()->getVulkanContext().getDevice().getVulkanDevice(), m_SwapChain, nullptr);
    });
    VKE_INFO("Created Swapchain");

    initImages();
}

VulkanSwapchain::~VulkanSwapchain()
{
}
}
