#include <EasyVulkan/Buffer.hpp>
#include <EasyVulkan/LogicalDevice.hpp>
#include <stdexcept>
#include "Buffer.hpp"

EasyVK::Buffer::Buffer(LogicalDevice *device, uint64_t size, bool deviceAccesible, VkBufferUsageFlags usage)
{
    _device = device;
    _size = size;

    VkBufferCreateInfo bufferCI{};
    bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCI.size = size;
    bufferCI.usage = usage;
    bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    _result = vkCreateBuffer(_device->getDevice(), &bufferCI, nullptr, &_buffer);

    if(_result != VK_SUCCESS){
        return;
    }

    VkMemoryRequirements memoryReq;
    vkGetBufferMemoryRequirements(_device->getDevice(), _buffer, &memoryReq);

    VkMemoryAllocateInfo allocI{};
    allocI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocI.allocationSize = memoryReq.size;
    allocI.memoryTypeIndex = _device->findMemoryType(memoryReq.memoryTypeBits, deviceAccesible ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: 0);

    _result = vkAllocateMemory(_device->getDevice(), &allocI, nullptr, &_mem);

    if(_result != VK_SUCCESS){
        vkDestroyBuffer(_device->getDevice(), _buffer, nullptr);
        return;
    }

    _result = vkBindBufferMemory(_device->getDevice(), _buffer, _mem, 0);

    if(_result != VK_SUCCESS){
        vkDestroyBuffer(_device->getDevice(), _buffer, nullptr);
        vkFreeMemory(_device->getDevice(), _mem, nullptr);
        return;
    }
}

EasyVK::Buffer::Buffer()
{
}

VkBuffer EasyVK::Buffer::getBuffer()
{
    return _buffer;
}

VkDeviceMemory EasyVK::Buffer::getMemory()
{
    return _mem;
}

VkResult EasyVK::Buffer::getResult()
{
    return _result;
}

uint32_t EasyVK::Buffer::getSize()
{
    return _size;
}

bool EasyVK::Buffer::isReady()
{
    return _result == VK_SUCCESS;
}

void *EasyVK::Buffer::map()
{
    void* ret;

    vkMapMemory(_device->getDevice(), _mem, 0, _size, 0, &ret);

    return ret;
}

void EasyVK::Buffer::unMap()
{
    vkUnmapMemory(_device->getDevice(), _mem);
}

void EasyVK::Buffer::destroy()
{
    if(isReady()){
        vkDestroyBuffer(_device->getDevice(), _buffer, nullptr);
        vkFreeMemory(_device->getDevice(), _mem, nullptr);
        _result = VK_NOT_READY;
    }
}