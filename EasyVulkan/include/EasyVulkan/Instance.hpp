#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <EasyVulkan/Version.hpp>
#include <EasyVulkan/QueueFamilies.hpp>

namespace EasyVK{

class PhysicalDevice;

class Instance{

private:
    VkInstance _instance = VK_NULL_HANDLE;
    bool _successful = false;
    VkResult _create_result = VK_NOT_READY;

public:
    Instance(
        const std::string& application_name, 
        const std::string& engine_name,
        const Version& application_version, 
        const Version& engine_version,
        const Version& vulkan_api_version,
        const std::vector<const char*>& enabled_extensions
    );

    Instance();

    bool valid();
    VkResult getResult();

    void init(
        const std::string& application_name, 
        const std::string& engine_name,
        const Version& application_version, 
        const Version& engine_version,
        const Version& vulkan_api_version,
        std::vector<const char*> enabled_extensions
    );

    VkInstance getHandledInstance();
    PhysicalDevice getPhysicalDevice(
            const std::vector<QueueThat> &supported_families,
            const std::vector<std::string>& supported_extensions,
            bool can_present,
            VkSurfaceKHR surface = VK_NULL_HANDLE
            );

    void destroy();
};
}