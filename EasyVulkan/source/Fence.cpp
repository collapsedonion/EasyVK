#include <EasyVulkan/Fence.hpp>
#include <EasyVulkan/LogicalDevice.hpp>

EasyVK::Fence::Fence(LogicalDevice *device, bool startSignaled)
{
    _device = device;

    VkFenceCreateInfo CI{};
    CI.sType =  VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if(startSignaled){
        CI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    
    _result = vkCreateFence(_device->getDevice(), &CI, nullptr, &_fence);
}

EasyVK::Fence::Fence()
{
}

VkFence EasyVK::Fence::getFence()
{
    return _fence;
}

VkResult EasyVK::Fence::getResult()
{
    return _result;
}

bool EasyVK::Fence::getStatus()
{
    return vkGetFenceStatus(_device->getDevice(), _fence) == VK_SUCCESS;
}

void EasyVK::Fence::waitForMe(uint64_t timeout)
{
    vkWaitForFences(_device->getDevice(), 1, &_fence, VK_TRUE, timeout);
}

void EasyVK::Fence::reset()
{
    vkResetFences(_device->getDevice(), 1, &_fence);
}

bool EasyVK::Fence::isReady()
{
    return _result == VK_SUCCESS;
}

void EasyVK::Fence::destroy()
{
    if(isReady()){
        _result = VK_NOT_READY;
        vkDestroyFence(_device->getDevice(), _fence, nullptr);
    }
}
