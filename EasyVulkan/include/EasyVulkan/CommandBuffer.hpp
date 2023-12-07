#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/LogicalDevice.hpp>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/Framebuffer.hpp>
#include <EasyVulkan/GraphicsPipeline.hpp>
#include <EasyVulkan/Buffer.hpp>
#include <EasyVulkan/Image.hpp>
#include <vector>

namespace EasyVK{

    class CommandPool;

    class CommandBuffer{
    private:
        LogicalDevice* _device;
        VkCommandPool _pool;
        VkCommandBuffer _buffer;
        VkResult _result = VK_NOT_READY;
        bool _ready = false;
        
        CommandBuffer(LogicalDevice * device, CommandPool* pool);

    public:
        CommandBuffer();

        VkCommandBuffer getBuffer();
        VkResult getResult();
        bool isReady();
        void destroy();

        VkResult begin(VkCommandBufferUsageFlags flags = 0);

        void beginRenderPass(RenderPass renderPass, Framebuffer framebuffer, VkRect2D renderArea, std::vector<VkClearValue> clearColors);
        void endRenderPass();
        void bindGraphicsPipeline(GraphicsPipeline pipeline);
        void bindGraphicsDescriptorSet(GraphicsPipeline pipeline, VkDescriptorSet set);
        void setViewports(std::vector<VkViewport> viewports);
        void setScissors(std::vector<VkRect2D> scissors);
        void bindBuffers(std::vector<Buffer> buffers, std::vector<VkDeviceSize> offsets);
        void bindIndexBuffer(Buffer buffer, VkIndexType type);
        void reset();
        void copyBuffer(Buffer src, Buffer dst, size_t size);
        void copyBufferToImage(Buffer src, Image dst);
        void imageLayoutTransition(Image img, VkImageLayout oldLayout, VkImageLayout newLayout);
        void draw(int count, int instances);
        void drawIndexed(uint32_t count, uint32_t instances);

        VkResult end();
    friend CommandPool;
    };
}
