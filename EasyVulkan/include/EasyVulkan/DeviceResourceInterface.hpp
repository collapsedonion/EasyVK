//
// Created by Роман  Тимофеев on 23.04.2024.
//

#ifndef EASY_VULKAN_TEST_DEVICERESOURCEINTERFACE_HPP
#define EASY_VULKAN_TEST_DEVICERESOURCEINTERFACE_HPP
#include <vulkan/vulkan.hpp>

namespace EasyVK{
    class ResourceDescriptor;

    class DeviceResource{

    public:
        enum ResourceTypeBits : uint32_t {
            AUTO = 0b1,
            STORAGE_BUFFER = 0b10,
            UNIFORM_BUFFER = 0b100,
            IMAGE_VIEW = 0b1000,
        };

        typedef uint32_t ResourceType;

    protected:
        virtual bool checkResourceTypeCompatability(ResourceType type) = 0;
        virtual void bindToDescriptorSet(vk::Device device, vk::DescriptorSet set, ResourceType type, uint32_t binding) = 0;
        virtual ResourceType getVerifiedResourceType(ResourceType type) = 0;
        vk::DescriptorType convertResourceTypeToDescriptorType(ResourceType type);
        vk::DescriptorSetLayoutBinding getBinding(uint32_t binding, ResourceType type);

    private:
        friend ResourceDescriptor;
    };
}

#endif //EASY_VULKAN_TEST_DEVICERESOURCEINTERFACE_HPP
