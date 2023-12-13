#include "EasyVulkan/Instance.hpp"
#include "EasyVulkan/PhysicalDevice.hpp"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

namespace EasyVK{
    Instance::Instance(
        const std::string& application_name,
        const std::string& engine_name,
        const Version& application_version,
        const Version& engine_version,
        const Version& vulkan_api_version,
        const std::vector<const char*>& enabled_extensions,
        bool enable_validation_layer
){
    init(application_name, engine_name, application_version, engine_version, vulkan_api_version, enabled_extensions, enable_validation_layer);
}

Instance::Instance()
{
}

bool Instance::valid()
{
    return _successful;
}

VkResult Instance::getResult()
{
    return _create_result;
}
void Instance::init(
    const std::string& application_name,
    const std::string& engine_name,
    const Version& application_version,
    const Version& engine_version,
    const Version& vulkan_api_version,
    std::vector<const char*> enabled_extensions,
    bool enable_Validation_Layers
)
{
    destroy();
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    applicationInfo.applicationVersion = application_version.getVulkanVersion();
    applicationInfo.engineVersion = engine_version.getVulkanVersion();
    applicationInfo.apiVersion = vulkan_api_version.getVulkanVersion(true);
    applicationInfo.pApplicationName = application_name.data();
    applicationInfo.pEngineName = engine_name.data();
    
    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;

    if (enable_Validation_Layers) {
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.enabledLayerCount = validationLayers.size();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    #ifdef __APPLE__
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        enabled_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif

    createInfo.enabledExtensionCount = enabled_extensions.size();
    createInfo.ppEnabledExtensionNames = enabled_extensions.data();

    _create_result = vkCreateInstance(&createInfo, nullptr, &_instance);

    if(_create_result == VK_SUCCESS){
        _successful = true;
    }
}

VkInstance Instance::getHandledInstance()
{
    return _instance;
}

PhysicalDevice Instance::getPhysicalDevice(            
    const std::vector<QueueThat> &supported_families,
    const std::vector<std::string>& supported_extensions,
    bool can_present,
    VkSurfaceKHR surface)
{
    if(_successful){
        return PhysicalDevice(*this, supported_families, supported_extensions, can_present, surface);
    }

    return PhysicalDevice();
}
void Instance::destroy()
{
    if(_successful){
        vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
        _successful = false;
        _create_result = VK_NOT_READY;
    }
}
}
