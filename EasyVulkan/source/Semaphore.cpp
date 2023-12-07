#include <EasyVulkan/Semaphore.hpp>
#include <EasyVulkan/LogicalDevice.hpp>
#include "Semaphore.hpp"

EasyVK::Semaphore::Semaphore(LogicalDevice *device)
{
    VkSemaphoreCreateInfo CI{};

    _device = device;

    CI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    _result = vkCreateSemaphore(_device->getDevice(), &CI, nullptr, &_semaphore);
}

EasyVK::Semaphore::Semaphore()
{
}

VkSemaphore EasyVK::Semaphore::getSemaphore()
{
    return _semaphore;
}

VkResult EasyVK::Semaphore::getResult()
{
    return _result;
}

bool EasyVK::Semaphore::isReady()
{
    return _result == VK_SUCCESS;
}

void EasyVK::Semaphore::destroy()
{
    if(isReady()){
        vkDestroySemaphore(_device->getDevice(), _semaphore, nullptr);
        _result = VK_NOT_READY;
    }
}
