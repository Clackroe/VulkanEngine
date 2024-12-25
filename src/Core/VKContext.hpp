#ifndef VKE_CONTEXT
#define VKE_CONTEXT
#include "Core/VKSwapChain.hpp"
#include <Core/VKDevice.hpp>
#include <core.hpp>
#include <vulkan/vulkan.h>

namespace VKE {
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NODEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class VKContext {

public:
    VKContext() = default;
    VKContext(const std::string& name);
    ~VKContext() { cleanup(); };

    void init();

    static void pushCleanupFunc(std::function<void()> func) { m_CleanupFunctions.push(func); };

    VkInstance& getVulkanInstance() { return m_VulkanInstance; }
    VulkanPhysicalDevice& getPhysicalDevice() { return *m_PhysicalDevice; }

    void update();

private:
    void createInstance();
    void setupValidationMsger();

    void cleanup();

private:
    std::string m_Name;

    inline static std::stack<std::function<void()>> m_CleanupFunctions;

    VkInstance m_VulkanInstance;

    Ref<VulkanPhysicalDevice> m_PhysicalDevice = nullptr;
    Ref<VulkanDevice> m_Device = nullptr;

    VkDebugUtilsMessengerEXT m_DebugMessenger;

    // Swapchain
    Ref<VulkanSwapchain> m_Swapchain = nullptr;
    //
    // Device
};
}

#endif
