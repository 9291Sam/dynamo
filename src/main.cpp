#include <iostream>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vkfw/vkfw.hpp>
#include <sebib/seblog.hpp>

int main()
{
    vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    static auto debugCallback = []VKAPI_ATTR VKAPI_CALL(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        [[maybe_unused]] void* pUserData)
        -> VkBool32 
    {
        seb::logFatal("Validation Layer Message: Severity: {} | Type: {} | \n{}",
            messageSeverity,
            messageType,
            pCallbackData->pMessage
        );

        return VK_FALSE;
    };

    const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo
    {
        .sType           {vk::StructureType::eDebugUtilsMessengerCreateInfoEXT},
        .pNext           {nullptr},
        .flags           {},
        .messageSeverity {
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        },
        .messageType     {
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        },
        .pfnUserCallback {debugCallback},
        .pUserData       {nullptr},
    };

    const vk::ApplicationInfo applicationInfo
    {
        .sType              {vk::StructureType::eApplicationInfo},
        .pNext              {nullptr},
        .pApplicationName   {"Dynamo"},
        .applicationVersion {
            VK_MAKE_API_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TWEAK)
        },
        .pEngineName        {"Dynamo"},
        .engineVersion      {
            VK_MAKE_API_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TWEAK)
        },
        .apiVersion         {VK_VERSION_1_0},
    };

    #ifndef NDEBUG
    #define VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS
    #endif // NDEBUG

    const std::vector<const char*> instanceLayers =
    []{
        #ifndef VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS

            auto availableLayers = vk::enumerateInstanceLayerProperties();

            if (std::find(availableLayers.cbegin(), availableLayers.cend(), 
                "VK_LAYER_KHRONOS_validation") == availableLayers.cend())
            {
                return std::vector<const char*> {};
            }

        #endif // VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS
        
        return std::vector<const char*> {"VK_LAYER_KHRONOS_validation"};
    }();

    const std::vector<const char*> instanceExtensions =
    []{
        std::vector<const char*> temp {}; 

        #ifdef __APPLE__
            temp.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            temp.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        #endif // __APPLE__

        #ifdef VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS
            temp.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            temp.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif // VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS

        auto vkfwRequiredExtensions = vkfw::getRequiredInstanceExtensions();
        temp.insert(
            temp.end(),
            vkfwRequiredExtensions.begin(),
            vkfwRequiredExtensions.end()
        );

        return temp;
    }();
    

    const vk::InstanceCreateInfo instanceCreateInfo
    {
        .sType                   {vk::StructureType::eInstanceCreateInfo},
        .pNext                   {
            #ifdef VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS
            reinterpret_cast<const void*>(&debugMessengerCreateInfo)
            #else
            nullptr
            #endif // VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS
        },
        .flags                   {
            #ifdef __APPLE__ 
                vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
            #endif // __APPLE__
        },
        .pApplicationInfo        {&applicationInfo},
        .enabledLayerCount       {static_cast<std::uint32_t>(instanceLayers.size())},
        .ppEnabledLayerNames     {instanceLayers.data()},
        .enabledExtensionCount   {static_cast<std::uint32_t>(instanceExtensions.size())},
        .ppEnabledExtensionNames {instanceExtensions.data()},
    };

    vk::UniqueInstance instance = vk::createInstanceUnique(instanceCreateInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);

    auto availablePhysicalDevices = instance->enumeratePhysicalDevices();
    seb::assertFatal(availablePhysicalDevices.size() > 0, "No vulkan devices are available");
    seb::logWarn("Picking first device TODO: select best gpu");

    const vk::DeviceCreateInfo deviceCreateInfo
    {
        .sType                   {vk::StructureType::eDeviceCreateInfo},
        .pNext                   {nullptr},
        .flags                   {},
        .queueCreateInfoCount    {},
        .pQueueCreateInfos       {},
        .enabledLayerCount       {},
        .ppEnabledLayerNames     {},
        .enabledExtensionCount   {},
        .ppEnabledExtensionNames {},
        .pEnabledFeatures        {},
    };

    availablePhysicalDevices.at(0).createDeviceUnique()
    // vk::Device device = physicalDevices[0].createDevice({}, nullptr);
    // VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

    seb::logLog("Hello World");
}