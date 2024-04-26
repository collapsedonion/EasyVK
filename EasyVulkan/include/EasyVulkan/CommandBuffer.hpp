//
// Created by Роман  Тимофеев on 21.04.2024.
//

#ifndef EASY_VULKAN_TEST_COMMANDBUFFER_HPP
#define EASY_VULKAN_TEST_COMMANDBUFFER_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/ComputePipeline.hpp>
#include <EasyVulkan/ResourceDescriptor.hpp>
#include <EasyVulkan/GraphicsPipeline.hpp>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/Buffer.hpp>

namespace EasyVK{
    class Device;

    class CommandBuffer : AutoFree{

    public:
        ~CommandBuffer();

    private:
        vk::Queue submitQueue;
        vk::Fence fence;
        vk::Semaphore mySemaphore;
        vk::CommandBuffer bufferHandler;
        vk::CommandPool myCommandPool;
        vk::Device device;

    protected:
        void allocateCommandBuffer(vk::CommandPool commandPool, vk::Queue submitQueue, vk::Device device);

    public:
        void begin();
        void end();
        void submit(const std::vector<CommandBuffer>& waitCommandBuffers = {});
        void reset();
        void bind(ComputePipeline pipeline);
        void bind(ComputePipeline pipeline, ResourceDescriptor descriptor);
        void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
        void beginRenderPass(RenderPass renderPass, uint32_t frameBufferIndex = 0);
        void bind(GraphicsPipeline graphicsPipeline);
        void bind(GraphicsPipeline graphicsPipeline, ResourceDescriptor descriptor);
        void bindVertexBuffer(EasyVK::Buffer buffer);
        void draw(uint32_t count, uint32_t instances);
        void setScissors(std::vector<vk::Rect2D> scissors);
        void setViewport(std::vector<vk::Viewport> viewports);
        void endRenderPass();
        void await();

    private:
        friend Device;
        friend SwapChain;
    };
}

#endif //EASY_VULKAN_TEST_COMMANDBUFFER_HPP
