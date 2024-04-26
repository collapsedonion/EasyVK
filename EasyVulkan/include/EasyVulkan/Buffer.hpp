//
// Created by Роман  Тимофеев on 22.04.2024.
//

#ifndef EASY_VULKAN_TEST_BUFFER_HPP
#define EASY_VULKAN_TEST_BUFFER_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/DeviceResourceInterface.hpp>
#include <EasyVulkan/Base.hpp>
#include <vector>
#include <cctype>

namespace EasyVK{

    class Device;

    class Buffer : AutoFree, public DeviceResource{

    public:
        ~Buffer();

    private:
        vk::Device device;
        vk::Buffer buffer;
        vk::DeviceMemory allocatedMemory;
        size_t allocatedSize = 0;
        bool hostVisible = false;

    public:
        void* bind();
        void unbind();

    protected:
        void allocate(vk::PhysicalDevice physicalDevice, vk::Device device, size_t size, bool hostVisible, const std::vector<uint32_t> &queueFamilies);

        bool checkResourceTypeCompatability(ResourceType type) override;
        void bindToDescriptorSet(vk::Device device, vk::DescriptorSet set, ResourceType type, uint32_t binding) override;
        ResourceType getVerifiedResourceType(EasyVK::DeviceResource::ResourceType type) override;

    private:
        friend Device;
        friend CommandBuffer;
    };
}

#endif //EASY_VULKAN_TEST_BUFFER_HPP
