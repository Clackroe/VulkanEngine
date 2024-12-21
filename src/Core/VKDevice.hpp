
#ifndef VKE_DEVICE
#define VKE_DEVICE
#include <core.hpp>
#include <vulkan/vulkan_core.h>

namespace VKE {
class VKEDevice {
public:
    VKEDevice();
    ~VKEDevice();

    void init(Ref<VkInstance> instance);

private:
    void pickPhysicalDevice();

private:
    VkPhysicalDevice m_Physical = VK_NULL_HANDLE;
    Ref<VkInstance> m_Intance;
};
}

#endif
