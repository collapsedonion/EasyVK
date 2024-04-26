//
// Created by Роман  Тимофеев on 22.04.2024.
//

#ifndef EASY_VULKAN_TEST_UTILS_HPP
#define EASY_VULKAN_TEST_UTILS_HPP
#include <vulkan/vulkan.hpp>

namespace EasyVK{
    vk::DeviceMemory allocateDeviceMemory(vk::Device device, vk::PhysicalDevice physicalDevice, vk::MemoryRequirements requirements, bool hostVisible);
}

#endif //EASY_VULKAN_TEST_UTILS_HPP
