#pragma once
#include <vulkan/vulkan.h>

namespace EasyVK{

    class LogicalDevice;

    class Semaphore{
    private:
        LogicalDevice* _device;
        VkSemaphore _semaphore;

        VkResult _result = VK_NOT_READY;

        Semaphore(LogicalDevice* device);
    public:    
        Semaphore();

        VkSemaphore getSemaphore();
        VkResult getResult();
        bool isReady();
        void destroy();

    friend LogicalDevice;
    };
}