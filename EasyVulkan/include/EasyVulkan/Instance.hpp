//
// Created by Роман  Тимофеев on 20.04.2024.
//

#ifndef EASY_VULKAN_TEST_INSTANCE_HPP
#define EASY_VULKAN_TEST_INSTANCE_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/Base.hpp>
#include <EasyVulkan/PhysicalDevice.hpp>
#include <set>

namespace EasyVK{
    class Instance{
    public:
        enum VULKAN_VERSION{
            V1_0
        };
    private:
        std::set<std::string> supported_layers;
        std::set<std::string> supported_extensions;
        vk::Instance instance;

    public:
        Instance(
                const std::string& application_name,
                VULKAN_VERSION version,
                const std::set<std::string>& supported_layers,
                const std::set<std::string>& supported_extensions
                );

        bool layerEnabled(const std::string& name);
        bool extensionEnabled(const std::string& name);

        PhysicalDevice* getPhysicalDevice(EasyVK::PhysicalDevice::DeviceType type, EasyVK::Features features);

    private:
        friend GLFWHelper;
    };
}

#endif //EASY_VULKAN_TEST_INSTANCE_HPP
