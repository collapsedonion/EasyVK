#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/Queue.hpp>
#include <EasyVulkan/CommandBuffer.hpp>

namespace EasyVK{

    class LogicalDevice;

    class CommandPool{

    private:
        VkCommandPool _pool;
        LogicalDevice* _device;
        Queue _queue;
        VkResult _result = VK_NOT_READY;
        bool _ready = false;

        CommandPool(Queue queue, LogicalDevice* device);

    public:
        CommandPool();

        void init();
        VkResult getResult();
        bool isReady();
        LogicalDevice getDevice();
        VkCommandPool getPool();
        void destroy();

        CommandBuffer createCommandBuffer();

    friend Queue;
    };
}