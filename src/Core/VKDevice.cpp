#include "Core/Application.hpp"
#include <Core/VKContext.hpp>
#include <Core/VKDevice.hpp>
#include <vulkan/vulkan_core.h>
namespace VKE {

namespace Utils {

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        VkPhysicalDeviceFeatures feats;
        vkGetPhysicalDeviceFeatures(device, &feats);

        return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    }

}

VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance& instance)
{
    pickPhysicalDevice(instance);
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}

void VulkanPhysicalDevice::getQueueFamilyIndicies(QueueFamilyIndicies* indicies, int requestedTypes)
{
    if (requestedTypes & VK_QUEUE_GRAPHICS_BIT) {
        for (u32 i = 0; i < m_QueueFamilyProps.size(); i++) {
            const auto& prop = m_QueueFamilyProps[i];
            if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indicies->graphics = i;
            }
        }
    }
    if (requestedTypes & VK_QUEUE_COMPUTE_BIT) {
        for (u32 i = 0; i < m_QueueFamilyProps.size(); i++) {
            const auto& prop = m_QueueFamilyProps[i];
            if (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indicies->compute = i;
            }
        }
    }
    if (requestedTypes & VK_QUEUE_TRANSFER_BIT) {
        for (u32 i = 0; i < m_QueueFamilyProps.size(); i++) {
            const auto& prop = m_QueueFamilyProps[i];
            if (prop.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                indicies->transfer = i;
            }
        }
    }
}

void VulkanPhysicalDevice::pickPhysicalDevice(VkInstance& instance)
{
    u32 devCount = 0;
    vkEnumeratePhysicalDevices(instance, &devCount, nullptr);

    VKE_ASSERT(devCount >= 0, "No GPUS??? You crazy???");

    std::vector<VkPhysicalDevice> devices(devCount);
    vkEnumeratePhysicalDevices(instance, &devCount, devices.data());

    for (const auto& dev : devices) {
        if (Utils::isDeviceSuitable(dev)) {
            m_Physical = dev;
            VKE_INFO("Physical Device Found!")
            break;
        }
    }

    VKE_ASSERT(m_Physical != VK_NULL_HANDLE, "No Suitable GPUS??? You crazy???");
    vkGetPhysicalDeviceProperties(m_Physical, &m_DeviceProps);
    vkGetPhysicalDeviceFeatures(m_Physical, &m_DeviceFeatures);

    VKE_INFO("PHYSICAL DEVICE: ")
    VKE_INFO("     {0}", m_DeviceProps.deviceName)
    // VKE_INFO("     {}")
    // VKE_INFO("    {}")

    u32 queueFamCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_Physical, &queueFamCount, nullptr);
    m_QueueFamilyProps.resize(queueFamCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_Physical, &queueFamCount, m_QueueFamilyProps.data());

    getQueueFamilyIndicies(&m_QueueFamilyIndicies, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
}

}
