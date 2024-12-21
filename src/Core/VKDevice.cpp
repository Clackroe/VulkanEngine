#include "Core/Application.hpp"
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

VKEDevice::VKEDevice()
{
}

void VKEDevice::init(Ref<VkInstance> instance)
{
    m_Intance = instance;
    pickPhysicalDevice();
}

VKEDevice::~VKEDevice()
{
}

void VKEDevice::pickPhysicalDevice()
{
    u32 devCount = 0;
    vkEnumeratePhysicalDevices(*m_Intance, &devCount, nullptr);
    VKE_ASSERT(devCount >= 0, "No GPUS??? You crazy???");

    std::vector<VkPhysicalDevice> devices(devCount);
    vkEnumeratePhysicalDevices(*m_Intance, &devCount, devices.data());

    for (const auto& dev : devices) {
        if (Utils::isDeviceSuitable(dev)) {
            m_Physical = dev;
            VKE_INFO("Physical Device Found!")
            break;
        }
    }

    VKE_ASSERT(m_Physical != VK_NULL_HANDLE, "No Suitable GPUS??? You crazy???");
}

}
