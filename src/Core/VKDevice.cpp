#include "Core/Application.hpp"
#include <Core/VKContext.hpp>
#include <Core/VKDevice.hpp>
#include <vulkan/vulkan_core.h>
namespace VKE {

namespace Utils {

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        VkPhysicalDeviceFeatures feats;
        vkGetPhysicalDeviceFeatures(device, &feats);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && extensionsSupported;
    }

}

// ================ PHYSICAL DEVICE =======================

VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance& instance)
{
    m_Instance = instance;
    pickPhysicalDevice(instance);
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}

void VulkanPhysicalDevice::findQueueFamilyIndicies(QueueFamilyIndicies* indicies, int requestedTypes)
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
    std::vector<VkQueueFamilyProperties> queueProps = getQueueFamilyProps();

    std::vector<VkBool32> supportsPresent(queueProps.size());
    for (u32 i = 0; i < queueProps.size(); i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(m_Physical, i, Application::getVulkanContext().getSurface(), &supportsPresent[i]);
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
    setPresentQueue(presentQueueNodeIndex);
}

void VulkanPhysicalDevice::setPresentQueue(u32 index)
{
    m_QueueFamilyIndicies.present = index;
};

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
    VKE_INFO("  {0}", m_DeviceProps.deviceName)

    u32 queueFamCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_Physical, &queueFamCount, nullptr);
    m_QueueFamilyProps.resize(queueFamCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_Physical, &queueFamCount, m_QueueFamilyProps.data());

    findQueueFamilyIndicies(&m_QueueFamilyIndicies, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
}

// ================ LOGICAL DEVICE =====================

VulkanDevice::VulkanDevice(const Ref<VulkanPhysicalDevice> physicalDevice)
{
    m_Physical = physicalDevice;

    auto& indices = m_Physical->getQueueIndices();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.resize(3);
    float queuePriority = 1.0f;
    if (indices.graphics != -1) {
        VkDeviceQueueCreateInfo& qInfo = queueCreateInfos[0];
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = indices.graphics;
        qInfo.queueCount = 1;
        qInfo.pQueuePriorities = &queuePriority;
    }
    if (indices.compute != -1) {
        VkDeviceQueueCreateInfo& qInfo = queueCreateInfos[1];
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = indices.compute;
        qInfo.queueCount = 1;
        qInfo.pQueuePriorities = &queuePriority;
    }
    if (indices.transfer != -1) {
        VkDeviceQueueCreateInfo& qInfo = queueCreateInfos[2];
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = indices.transfer;
        qInfo.queueCount = 1;
        qInfo.pQueuePriorities = &queuePriority;
    }
    if (indices.present != -1) {
        VkDeviceQueueCreateInfo& qInfo = queueCreateInfos[3];
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = indices.present;
        qInfo.queueCount = 1;
        qInfo.pQueuePriorities = &queuePriority;
    }

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &m_Physical->getFeats();

    createInfo.enabledExtensionCount = Utils::deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = Utils::deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (u32)(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult res = vkCreateDevice(m_Physical->getVulkanPhysical(), &createInfo, nullptr, &m_Device);
    VKE_ASSERT(res == VK_SUCCESS, "Failed to create Vulkan Logical Device")
    VKE_INFO("Created Logical Device");

    VKContext::pushCleanupFunc([=]() { vkDestroyDevice(m_Device, nullptr); });

    if (indices.graphics != -1) {
        vkGetDeviceQueue(m_Device, indices.graphics, 0, &m_Queues[0]);
    }
    if (indices.compute != -1) {
        vkGetDeviceQueue(m_Device, indices.compute, 0, &m_Queues[1]);
    }
    if (indices.transfer != -1) {
        vkGetDeviceQueue(m_Device, indices.transfer, 0, &m_Queues[2]);
    }
    if (indices.present != -1) {
        vkGetDeviceQueue(m_Device, indices.present, 0, &m_Queues[3]);
    } else {
        VKE_WARN("No Present Queue Index provided. Are you sure your device supports presentation?")
    }
}

VulkanDevice::~VulkanDevice()
{
}

}
