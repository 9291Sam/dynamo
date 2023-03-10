#include "includes.hpp"

#include <sebib/seblog.hpp>

#include "instance.hpp"




#ifndef NDEBUG

    #define VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS

    static VkBool32 debugMessageCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        [[maybe_unused]] void* pUserData)
    {
        seb::panic("Validation Layer Message: Severity: {} | Type: {} | \n{}",
            messageSeverity,
            messageType,
            pCallbackData->pMessage
        );

        return VK_FALSE;
    }

    // The below two functions vk{Create, Destroy}DebugUtilsMessengerEXT must by
    // aren't loaded yet. Only vkGetInstanceProcAddr so we have to bootstrap
    // the instance so it can init everything else 
    [[nodiscard]] static auto dynVkCreateDebugUtilsMessengerEXT(
        PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr,
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pMessenger)
        -> VkResult
    {
        const auto maybeVkCreateDebugUtilsMessengerExt =
            reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                dynVkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
            );
        
        seb::assertFatal(
            maybeVkCreateDebugUtilsMessengerExt != nullptr,
            "Failed to dyn load vkCreateDebugUtilsMessengerEXT!"
        );

        return maybeVkCreateDebugUtilsMessengerExt(
            instance,
            pCreateInfo, 
            pAllocator,
            pMessenger
        );
    }

    static void dynVkDestroyDebugUtilsMessengerEXT(
        PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr,
        VkInstance instance,
        VkDebugUtilsMessengerEXT messenger,
        [[maybe_unused]] const VkAllocationCallbacks* pAllocator)
    {
        const auto maybeVkDestroyDebugUtilsMessengerEXT =
            reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                dynVkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
            );
        
        seb::assertFatal(
            maybeVkDestroyDebugUtilsMessengerEXT != nullptr,
            "Failed to dy load vkDestroyDebugUtilsMessengerEXT!"
        );

        // Function always succeeds
        maybeVkDestroyDebugUtilsMessengerEXT(
            instance,
            messenger, 
            pAllocator
        );
    }

#endif // NDEBUG



namespace render
{
    Instance::Instance(PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr)
        : dyn_vk_get_instance_proc_addr {dynVkGetInstanceProcAddr}
    {
        const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo
        {
            .sType           {vk::StructureType::eDebugUtilsMessengerCreateInfoEXT},
            .pNext           {nullptr},
            .flags           {},
            .messageSeverity {
                // vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                // vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
            },
            .messageType     {
                // vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
            },
            .pfnUserCallback {debugMessageCallback},
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
            .apiVersion         {VK_API_VERSION_1_2},
        };

        const std::vector<const char*> instanceLayers =
        []{
            #ifdef VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS

                for (vk::LayerProperties layer : vk::enumerateInstanceLayerProperties())
                {
                    if (std::strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)
                    {
                        return std::vector<const char*> {"VK_LAYER_KHRONOS_validation"};
                    }
                }

            #endif // VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS
            
            return std::vector<const char*> {};
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
                // putting this pointer here allows the instance to debug itself
                // validation layers are fully setup
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

        this->instance = vk::createInstanceUnique(instanceCreateInfo);

        const VkDebugUtilsMessengerCreateInfoEXT copyCreateInfo = debugMessengerCreateInfo;

        #ifdef VULKAN_INSTANCE_ENABLE_VALIDATION_LAYERS

            seb::assertFatal(
                dynVkCreateDebugUtilsMessengerEXT(
                    this->dyn_vk_get_instance_proc_addr,
                    static_cast<VkInstance>(*this->instance),
                    &copyCreateInfo, // this comes from the first line 
                    nullptr,
                    &this->debug_messenger
                ) == VK_SUCCESS,
                "Failed to initialize debug Messenger"
            );

        #endif

        
        seb::logLog("Vulkan initalized successfully");
    }

    Instance::~Instance()
    {
        dynVkDestroyDebugUtilsMessengerEXT(
            this->dyn_vk_get_instance_proc_addr,
            *this->instance, 
            this->debug_messenger,
            nullptr
        );
    }

    vk::Instance Instance::operator*() const
    {
        return *this->instance;
    }
} // namespace render
