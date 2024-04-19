#include <EasyVulkan/CommandPool.hpp>
#include <EasyVulkan/Queue.hpp>
#include <EasyVulkan/LogicalDevice.hpp>


EasyVK::CommandPool::CommandPool(Queue queue, LogicalDevice *device)
{
    _queue = queue;
    _device = device;
}

EasyVK::CommandPool::CommandPool()
{
}

void EasyVK::CommandPool::init()
{
    destroy();

    if(_device == nullptr){
        return;
    }

    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = _queue.getFamily();

    _result = vkCreateCommandPool(_device->getDevice(), &createInfo, nullptr, &_pool);
    _ready = _result == VK_SUCCESS;
}

VkResult EasyVK::CommandPool::getResult()
{
    return _result;
}

bool EasyVK::CommandPool::isReady()
{
    return _ready;
}

EasyVK::LogicalDevice EasyVK::CommandPool::getDevice()
{
    return *_device;
}

VkCommandPool EasyVK::CommandPool::getPool()
{
    return _pool;
}

void EasyVK::CommandPool::destroy()
{
    if(_ready){
        _ready = false;
        _result = VK_NOT_READY;
        vkDestroyCommandPool(_device->getDevice(), _pool, nullptr);
    }
}

EasyVK::CommandBuffer EasyVK::CommandPool::createCommandBuffer()
{
    return CommandBuffer(_device, this);
}
