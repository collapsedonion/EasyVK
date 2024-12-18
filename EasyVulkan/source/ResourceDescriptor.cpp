//
// Created by Роман  Тимофеев on 23.04.2024.
//

#include <EasyVulkan/ResourceDescriptor.hpp>
#include <stdexcept>

void EasyVK::ResourceDescriptor::setup(vk::Device device,
                                       const std::map<uint32_t, ResourceBindingDescriptor> &bindings) {
    allocateDescriptorPool(device, bindings);
    this->device = device;
    for(const auto& binding : bindings){
        updateBinding(binding.first, binding.second.resource);
    }
}

void EasyVK::ResourceDescriptor::allocateDescriptorPool(vk::Device device,
                                                         const std::map<uint32_t, ResourceBindingDescriptor> &bindings) {

    if(this->descriptorPool != VK_NULL_HANDLE){
        this->bindingTypes.clear();
        device.destroy(this->descriptorPool);
        device.destroy(this->descriptorLayout);
    }

    vk::DescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    poolCreateInfo.maxSets = 1;

    std::vector<vk::DescriptorPoolSize> typesToInclude;
    typesToInclude.reserve(bindings.size());

    for(auto elem : bindings){
        if(!elem.second.resource->checkResourceTypeCompatability(elem.second.type)){
            throw std::runtime_error("Unsupported resource type");
        }

        vk::DescriptorPoolSize newSize;
        newSize.descriptorCount = 1;
        newSize.type = elem.second.resource->convertResourceTypeToDescriptorType(elem.second.type);
        typesToInclude.push_back(newSize);
    }

    poolCreateInfo.poolSizeCount = typesToInclude.size();
    poolCreateInfo.pPoolSizes = typesToInclude.data();

    descriptorPool = device.createDescriptorPool(poolCreateInfo);

    std::vector<vk::DescriptorSetLayoutBinding> bindingsForCreateInfo;
    bindingsForCreateInfo.reserve(bindings.size());

    for(const auto& binding : bindings){
        bindingsForCreateInfo.push_back(binding.second.resource->getBinding(binding.first, binding.second.type));
        this->bindingTypes.insert({binding.first, binding.second.resource->getVerifiedResourceType(binding.second.type)});
    }

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
    descriptorSetLayoutCreateInfo.bindingCount = bindingsForCreateInfo.size();
    descriptorSetLayoutCreateInfo.pBindings = bindingsForCreateInfo.data();

    descriptorLayout = device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

    vk::DescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
    allocateInfo.descriptorPool = this->descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &this->descriptorLayout;

    descriptorSet = device.allocateDescriptorSets({allocateInfo})[0];
}

void EasyVK::ResourceDescriptor::updateBinding(uint32_t binding, EasyVK::DeviceResource* resource) {
    if(resource->checkResourceTypeCompatability(this->bindingTypes[binding])){
        resource->bindToDescriptorSet(this->device, this->descriptorSet, this->bindingTypes[binding], binding);
    }
}

EasyVK::ResourceDescriptor::~ResourceDescriptor() {
    device.destroy(this->descriptorLayout);
    device.destroy(this->descriptorPool);
}
