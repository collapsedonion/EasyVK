#pragma once
#include <vulkan/vulkan.h>

namespace EasyVK{

    class LogicalDevice;

    class Fence{
    private:
        LogicalDevice* _device;
        VkFence _fence;

        VkResult _result = VK_NOT_READY;

        Fence(LogicalDevice* device, bool startSignaled);
    public:
        Fence();

        VkFence getFence();
        VkResult getResult();
        bool getStatus();
        void waitForMe(uint64_t timeout);
        void reset();
        bool isReady();

        void destroy();

    friend LogicalDevice;
    };
}