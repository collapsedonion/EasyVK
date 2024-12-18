//
// Created by Роман  Тимофеев on 23.04.2024.
//

#ifndef EASY_VULKAN_TEST_RESOURCEDESCRIPTOR_HPP
#define EASY_VULKAN_TEST_RESOURCEDESCRIPTOR_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/DeviceResourceInterface.hpp>
#include <EasyVulkan/Base.hpp>
#include <map>

namespace EasyVK{

    class ResourceDescriptor{

    public:
        struct ResourceBindingDescriptor{
            DeviceResource* resource;
            DeviceResource::ResourceType type = DeviceResource::AUTO;
        };

    public:
        ~ResourceDescriptor();

    private:
        vk::Device device;
        vk::DescriptorPool descriptorPool = VK_NULL_HANDLE;
        vk::DescriptorSetLayout descriptorLayout = VK_NULL_HANDLE;
        vk::DescriptorSet descriptorSet = VK_NULL_HANDLE;
        std::map<uint32_t, DeviceResource::ResourceType> bindingTypes;

    public:
        void updateBinding(uint32_t binding, DeviceResource* resource);

    private:
        void allocateDescriptorPool(vk::Device device,
                                     const std::map<uint32_t, ResourceBindingDescriptor> &bindings);

        void setup(vk::Device device,
                   const std::map<uint32_t, ResourceBindingDescriptor> &bindings);

    private:
        friend Device;
        friend ComputePipeline;
        friend CommandBuffer;
        friend GraphicsPipeline;
    };
};

#endif //EASY_VULKAN_TEST_RESOURCEDESCRIPTOR_HPP
