#include "Core/VKDevice.hpp"
#include "Core/VKSwapChain.hpp"
#include "GLFW/glfw3.h"
#include "core.hpp"
#include <Core/VKContext.hpp>
#include <bits/types/wint_t.h>
#include <functional>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace VKE {

const std::vector<const char*> wantedInstanceExtensions = {};

namespace Utils {

    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            VKE_INFO("VALIDATION: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            VKE_WARN("VALIDATION: {}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            VKE_ERROR("VALIDATION: {}", pCallbackData->pMessage);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            break;
        }

        return VK_FALSE;
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    bool checkValidationLayerSupport()
    {
        u32 layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> layerProps(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layerProps.data());

        bool found = false;
        for (const char* layerName : VKE::validationLayers) {

            for (const auto& layerProperties : layerProps) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> getReqInstanceExt()
    {
        u32 glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extenstions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        VKE_INFO("Extensions GLFW: ")
        for (auto ext : extenstions) {
            VKE_INFO("    {}", ext);
        }

        if (enableValidationLayers) {
            extenstions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        for (auto t : wantedInstanceExtensions) {
            extenstions.push_back(t);
        }
        return extenstions;
    }
}

VKContext::VKContext(const std::string& name) {};

void VKContext::init()
{
    createInstance();
    setupValidationMsger();
    m_PhysicalDevice = CreateRef<VulkanPhysicalDevice>(m_VulkanInstance);
    m_Swapchain = CreateRef<VulkanSwapchain>(m_VulkanInstance);
    m_Device = CreateRef<VulkanDevice>(m_PhysicalDevice);
    VKE_INFO("Context Initialized");
}

void VKContext::createInstance()
{

    VKE_ASSERT(enableValidationLayers && Utils::checkValidationLayerSupport(), "validationLayers requested but not supported")

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pEngineName = m_Name.c_str();
    appInfo.pEngineName = "Unnamed";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceInfo {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    u32 glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> reqExtensions;

    for (u32 i = 0; i < glfwExtensionCount; i++) {
        reqExtensions.emplace_back(glfwExtensions[i]);
    }

// MACOS Only
#ifdef VKE_MACOS
    reqExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    auto extenstions = Utils::getReqInstanceExt();
    instanceInfo.enabledExtensionCount = (u32)extenstions.size();
    instanceInfo.ppEnabledExtensionNames = extenstions.data();
    VKE_INFO("Extensions: ")
    for (auto ext : extenstions) {
        VKE_INFO("    {}", ext);
    }

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    if (enableValidationLayers) {
        instanceInfo.enabledLayerCount = (u32)validationLayers.size();
        instanceInfo.ppEnabledLayerNames = validationLayers.data();

        Utils::populateDebugMessengerCreateInfo(debugCreateInfo);
        instanceInfo.pNext = &debugCreateInfo;
    } else {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_VulkanInstance);
    VKE_ASSERT(result == VK_SUCCESS, "Failed to create instace")
    VKContext::pushCleanupFunc([&]() {
        vkDestroyInstance(m_VulkanInstance, nullptr);
    });
}

void VKContext::setupValidationMsger()
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo {};
    Utils::populateDebugMessengerCreateInfo(createInfo);

    VKE_ASSERT(Utils::createDebugUtilsMessengerEXT(m_VulkanInstance, &createInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to create debugCallback");

    VKContext::pushCleanupFunc([&]() { Utils::destroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr); });
}

void VKContext::update()
{
}

void VKContext::cleanup()
{
    std::function<void()> func = m_CleanupFunctions.top();
    while (func) {
        func();
        m_CleanupFunctions.pop();
        func = m_CleanupFunctions.top();
    }
}

}
