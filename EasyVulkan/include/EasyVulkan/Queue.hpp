#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/QueueFamilies.hpp>
#include <EasyVulkan/Fence.hpp>
#include <EasyVulkan/Semaphore.hpp>
#include <vector>

namespace EasyVK{

    class LogicalDevice;
    class CommandPool;
    class CommandBuffer;
    class SwapChain;

    class Queue{

    private:
        VkQueue _queue = VK_NULL_HANDLE;
        QueueThat _flags = QueueThat::ZERO;
        bool canPresent = false;
        unsigned int family = 0;
        LogicalDevice* _log_device = nullptr; 

    public:
        VkQueue getQueue();
        QueueThat getFlags();
        bool getCanPresent();
        unsigned int getFamily();
        CommandPool createCommandPool();
        bool isValid();
        LogicalDevice getLogicalDevice();
        VkResult submit(
            std::vector<CommandBuffer> buffersToSubmit,
            Fence fence = {},
            std::vector<Semaphore> semaphoresToWait = {},
            std::vector<VkPipelineStageFlags> waitStage = {},
            std::vector<Semaphore> signalSemaphores = {}
        );
        VkResult presentSwapChain(SwapChain swapChain, uint32_t imageIndex, std::vector<Semaphore> waitSemaphores);

    friend LogicalDevice;
    };
}