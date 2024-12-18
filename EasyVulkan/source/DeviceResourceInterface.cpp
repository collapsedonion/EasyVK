//
// Created by Роман  Тимофеев on 23.04.2024.
//

#include <EasyVulkan/DeviceResourceInterface.hpp>
#include <stdexcept>

vk::DescriptorType
EasyVK::DeviceResource::convertResourceTypeToDescriptorType(EasyVK::DeviceResource::ResourceType type) {

    type = getVerifiedResourceType(type);

    switch (type) {
        case STORAGE_BUFFER:
            return vk::DescriptorType::eStorageBuffer;
        case UNIFORM_BUFFER:
            return vk::DescriptorType::eUniformBuffer;
        case IMAGE_VIEW:
            return vk::DescriptorType::eCombinedImageSampler;

        default:
            break;
    }

    throw std::runtime_error("Invalid resource type");
}

vk::DescriptorSetLayoutBinding
EasyVK::DeviceResource::getBinding(uint32_t binding, EasyVK::DeviceResource::ResourceType type) {
    vk::DescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = convertResourceTypeToDescriptorType(type);
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = vk::ShaderStageFlagBits::eAll;
    return layoutBinding;
}
