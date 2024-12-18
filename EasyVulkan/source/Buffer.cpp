//
// Created by Роман  Тимофеев on 22.04.2024.
//
#include <EasyVulkan/Buffer.hpp>
#include "Utils.hpp"

void EasyVK::Buffer::allocate(vk::PhysicalDevice physicalDevice, vk::Device device, size_t size, bool hostVisible,
                              const std::vector<uint32_t> &queueFamilies) {

    vk::BufferUsageFlags flags;
    vk::SharingMode sharingMode = queueFamilies.size() > 1 ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;

    if(hostVisible){
        flags = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eStorageBuffer;
    }else{
        flags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eVertexBuffer;
    }

    vk::BufferCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eBufferCreateInfo;
    createInfo.sharingMode = sharingMode;
    createInfo.usage = flags;
    createInfo.size = size;
    createInfo.queueFamilyIndexCount = queueFamilies.size();
    createInfo.pQueueFamilyIndices = queueFamilies.data();

    vk::Buffer newBuffer = device.createBuffer(createInfo);
    vk::MemoryRequirements bufferMemoryRequirements = device.getBufferMemoryRequirements(newBuffer);
    vk::DeviceMemory bufferMemory = allocateDeviceMemory(device, physicalDevice, bufferMemoryRequirements, hostVisible);

    device.bindBufferMemory(newBuffer, bufferMemory,0);

    this->allocatedSize = bufferMemoryRequirements.size;
    this->device = device;
    this->hostVisible = hostVisible;
    this->buffer = newBuffer;
    this->allocatedMemory = bufferMemory;
}

EasyVK::Buffer::~Buffer() {
    this->device.freeMemory(this->allocatedMemory);
    this->device.destroy(this->buffer);   
}

void *EasyVK::Buffer::bind() {
    return device.mapMemory(this->allocatedMemory, 0, this->allocatedSize);
}

void EasyVK::Buffer::unbind() {
    device.unmapMemory(this->allocatedMemory);
}

bool EasyVK::Buffer::checkResourceTypeCompatability(EasyVK::DeviceResource::ResourceType type) {
    return (type & DeviceResource::UNIFORM_BUFFER) || (type & DeviceResource::STORAGE_BUFFER) || (type & DeviceResource::AUTO);
}

void
EasyVK::Buffer::bindToDescriptorSet(vk::Device device, vk::DescriptorSet set, ResourceType type, uint32_t binding) {

    vk::DescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = this->buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = this->allocatedSize;

    vk::WriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = convertResourceTypeToDescriptorType(type);
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.dstSet = set;
    writeDescriptorSet.pBufferInfo = &bufferInfo;

    device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

EasyVK::DeviceResource::ResourceType
EasyVK::Buffer::getVerifiedResourceType(EasyVK::DeviceResource::ResourceType type) {
    if(type & DeviceResource::AUTO){
        return DeviceResource::STORAGE_BUFFER;
    }

    return type;
}
