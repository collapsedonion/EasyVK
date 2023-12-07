#pragma once
#include <vulkan/vulkan.h>

namespace EasyVK{


    class DescriptorLayout{
    private:
        VkDescriptorSetLayoutBinding binding;

    public:
        DescriptorLayout();
        VkDescriptorSetLayoutBinding getBinding();

        static DescriptorLayout getUniformBufferBinding(uint32_t binding, VkShaderStageFlags stages);
        static DescriptorLayout getTextureSamplerLayoutBinding(uint32_t binding, VkShaderStageFlags stages);
    };
}