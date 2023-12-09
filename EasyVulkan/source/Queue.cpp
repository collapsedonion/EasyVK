#include "Queue.hpp"
#include <EasyVulkan/CommandPool.hpp>
#include <EasyVulkan/CommandBuffer.hpp>
#include <EasyVulkan/SwapChain.hpp>

VkQueue EasyVK::Queue::getQueue()
{
    return _queue;
}

EasyVK::QueueThat EasyVK::Queue::getFlags()
{
    return _flags;
}

bool EasyVK::Queue::getCanPresent()
{
    return canPresent;
}

unsigned int EasyVK::Queue::getFamily()
{
    return family;
}

EasyVK::CommandPool EasyVK::Queue::createCommandPool()
{
    CommandPool pool(*this, _log_device);
    pool.init();
    return pool;
}

VkResult EasyVK::Queue::wait()
{
    return vkQueueWaitIdle(_queue);
}

bool EasyVK::Queue::isValid()
{
    return _queue != VK_NULL_HANDLE;
}

EasyVK::LogicalDevice EasyVK::Queue::getLogicalDevice()
{
    return *_log_device;
}

VkResult EasyVK::Queue::submit(
    std::vector<CommandBuffer> buffersToSubmit, 
    Fence fence, 
    std::vector<Semaphore> semaphoresToWait, 
    std::vector<VkPipelineStageFlags> waitStage, 
    std::vector<Semaphore> signalSemaphores)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = buffersToSubmit.size();
    std::vector<VkCommandBuffer> commandBuffers;

    for(auto elem : buffersToSubmit){
        commandBuffers.push_back(elem.getBuffer());
    }

    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.pWaitDstStageMask = waitStage.data();
    submitInfo.waitSemaphoreCount = semaphoresToWait.size();

    std::vector<VkSemaphore> wSem;
    for(auto elem : semaphoresToWait){
        wSem.push_back(elem.getSemaphore());
    }
    submitInfo.pWaitSemaphores = wSem.data();
    submitInfo.signalSemaphoreCount = signalSemaphores.size();
    std::vector<VkSemaphore> sSem;
    for(auto elem : signalSemaphores){
        sSem.push_back(elem.getSemaphore());
    }
    submitInfo.pSignalSemaphores = sSem.data();
    return vkQueueSubmit(_queue, 1, &submitInfo, fence.isReady() ? fence.getFence() : VK_NULL_HANDLE);
}

VkResult EasyVK::Queue::presentSwapChain(SwapChain swapChain, uint32_t imageIndex, std::vector<Semaphore> waitSemaphores)
{
    VkPresentInfoKHR pI{};
    pI.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    VkSwapchainKHR sc = swapChain.getSwapchain();
    pI.pSwapchains =  &sc;
    pI.swapchainCount = 1;
    pI.pResults = nullptr;
    pI.pImageIndices = &imageIndex;

    std::vector<VkSemaphore> sems;
    for(auto elem : waitSemaphores){
        sems.push_back(elem.getSemaphore());
    }
    pI.pWaitSemaphores = sems.data();
    pI.waitSemaphoreCount = sems.size();
    return vkQueuePresentKHR(_queue, &pI);
}
