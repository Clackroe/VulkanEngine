#ifndef VKE_DEVICE
#define VKE_DEVICE
#include <core.hpp>
#include <vulkan/vulkan_core.h>

namespace VKE {

struct QueueFamilyIndicies {
    u32 graphics = -1;
    u32 compute = -1;
    u32 transfer = -1;
};

class VulkanPhysicalDevice {
public:
    VulkanPhysicalDevice(VkInstance& instance);
    ~VulkanPhysicalDevice();

    const VkPhysicalDeviceProperties& getProps() const { return m_DeviceProps; }
    const VkPhysicalDeviceFeatures& getFeats() const { return m_DeviceFeatures; }

private:
    void pickPhysicalDevice(VkInstance& instance);

    void getQueueFamilyIndicies(QueueFamilyIndicies* indicies, int requestedTypes);

private:
    VkPhysicalDevice m_Physical = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties m_DeviceProps;
    VkPhysicalDeviceFeatures m_DeviceFeatures;

    std::vector<VkQueueFamilyProperties> m_QueueFamilyProps;
    QueueFamilyIndicies m_QueueFamilyIndicies;
};

class VulkanDevice {
public:
    VulkanDevice(const Ref<VulkanPhysicalDevice> physicalDevice);
    ~VulkanDevice();

    const Ref<VulkanPhysicalDevice> getPhysicalDevice() const { return m_Physical; }
    const VkPhysicalDeviceProperties& getDeviceProps() const { return m_Physical->getProps(); }
    const VkPhysicalDeviceFeatures& getDeviceFeats() const { return m_Physical->getFeats(); }

private:
    Ref<VulkanPhysicalDevice> m_Physical = nullptr;
};

}

#endif
