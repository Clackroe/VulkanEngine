#ifndef VKE_DEVICE
#define VKE_DEVICE
#include <core.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace VKE {

struct QueueFamilyIndicies {
    u32 graphics = -1;
    u32 compute = -1;
    u32 transfer = -1;

    // Required
    u32 present = -1;
};

class VulkanPhysicalDevice {
public:
    VulkanPhysicalDevice(VkInstance& instance);
    ~VulkanPhysicalDevice();

    const VkPhysicalDeviceProperties& getProps() const { return m_DeviceProps; }
    const VkPhysicalDeviceFeatures& getFeats() const { return m_DeviceFeatures; }

    const QueueFamilyIndicies& getQueueIndices() const { return m_QueueFamilyIndicies; };
    const VkPhysicalDevice& getVulkanPhysical() const { return m_Physical; };

    const std::vector<VkQueueFamilyProperties>& getQueueFamilyProps() const { return m_QueueFamilyProps; }

    void setPresentQueue(u32 index);

private:
    void pickPhysicalDevice(VkInstance& instance);

    void findQueueFamilyIndicies(QueueFamilyIndicies* indicies, int requestedTypes);

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

    const VkQueue& getGraphicsQueue() const { return m_Queues[0]; }
    const VkQueue& getComputeQueue() const { return m_Queues[1]; }
    const VkQueue& getTransferQueue() const { return m_Queues[2]; }
    const VkQueue& getPresentQueue() const { return m_Queues[3]; }

private:
    Ref<VulkanPhysicalDevice> m_Physical = nullptr;
    VkDevice m_Device = VK_NULL_HANDLE;

    std::vector<VkQueue> m_Queues = std::vector<VkQueue>(4); // Graphics, Compute, Transfer, Present
};

}

#endif
