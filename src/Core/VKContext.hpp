#ifndef VKE_CONTEXT
#define VKE_CONTEXT
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
    VKContext(const std::string& name);
    ~VKContext() { cleanup(); };

    void pushCleanupFunc(std::function<void()> func) { m_CleanupFunctions.push(func); };

    Ref<VkInstance> getInstance() { return m_Instance; }

    void update();

private:
    void createInstance();
    void setupValidationMsger();

    void cleanup();

private:
    std::string m_Name;

    std::stack<std::function<void()>> m_CleanupFunctions;

    Ref<VkInstance> m_Instance = CreateRef<VkInstance>();

    VKEDevice m_Device;

    VkDebugUtilsMessengerEXT m_DebugMessenger;

    // Swapchain
    //
    // Device
};
}

#endif
