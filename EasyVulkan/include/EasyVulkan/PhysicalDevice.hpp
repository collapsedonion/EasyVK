//
// Created by Роман  Тимофеев on 20.04.2024.
//

#ifndef EASY_VULKAN_TEST_PHYSICALDEVICE_HPP
#define EASY_VULKAN_TEST_PHYSICALDEVICE_HPP
#include <vulkan/vulkan.hpp>
#include <cctype>
#include <EasyVulkan/DeviceFeatures.hpp>
#include <EasyVulkan/Device.hpp>
#include <vector>

namespace EasyVK{
    class Instance;

    class PhysicalDevice {
    public:
        enum DeviceTypeBits{
            ANY,
            DISCRETE,
            INTEGRATED
        };

        typedef uint64_t DeviceType;

    public:
        Device createDevice(vk::SurfaceKHR surface = VK_NULL_HANDLE);

    private:
        Features enabledFeatures = 0x0;
        vk::Instance instance;
        vk::PhysicalDevice deviceHandler;
        bool set_up = false;

    protected:
        void setupWithDesired(DeviceType type, Features features, vk::Instance instance);

    private:
        friend Instance;
    };
}

#endif //EASY_VULKAN_TEST_PHYSICALDEVICE_HPP
