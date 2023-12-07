#include <EasyVulkan/DescriptorLayout.hpp>
#include "DescriptorLayout.hpp"

EasyVK::DescriptorLayout::DescriptorLayout()
{
}

VkDescriptorSetLayoutBinding EasyVK::DescriptorLayout::getBinding()
{
    return this->binding;
}

EasyVK::DescriptorLayout EasyVK::DescriptorLayout::getUniformBufferBinding(uint32_t binding, VkShaderStageFlags stages)
{
    DescriptorLayout toRet{};

    toRet.binding.binding = binding;
    toRet.binding.descriptorCount = 1;
    toRet.binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    toRet.binding.stageFlags = stages;
    toRet.binding.pImmutableSamplers = nullptr;

    return toRet;
}

EasyVK::DescriptorLayout EasyVK::DescriptorLayout::getTextureSamplerLayoutBinding(uint32_t binding, VkShaderStageFlags stages)
{
    DescriptorLayout toRet{};

    toRet.binding.binding = binding;
    toRet.binding.descriptorCount = 1;
    toRet.binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    toRet.binding.stageFlags = stages;
    toRet.binding.pImmutableSamplers = nullptr;

    return toRet;
}
