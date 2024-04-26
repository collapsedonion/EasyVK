//
// Created by Роман  Тимофеев on 22.04.2024.
//
#include "Utils.hpp"
#include <cctype>
#include <stdexcept>

vk::DeviceMemory EasyVK::allocateDeviceMemory(vk::Device device, vk::PhysicalDevice physicalDevice, vk::MemoryRequirements requirements, bool hostVisible){
    vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = physicalDevice.getMemoryProperties();
    vk::MemoryPropertyFlags memoryFlags = hostVisible ? vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent : vk::MemoryPropertyFlagBits::eDeviceLocal;
    uint32_t memoryTypeIndex;
    bool found = false;

    for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++){
        vk::MemoryType processingMemoryType = deviceMemoryProperties.memoryTypes[i];
        if(((1 << i) & requirements.memoryTypeBits) && ((processingMemoryType.propertyFlags & memoryFlags) == memoryFlags)){
            memoryTypeIndex = i;
            found = true;
            break;
        }
    }

    if(!found){
        throw std::runtime_error("Can't find suitable device memory type");
    }

    vk::MemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = vk::StructureType::eMemoryAllocateInfo;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    allocateInfo.allocationSize = requirements.size;
    return device.allocateMemory(allocateInfo);
}