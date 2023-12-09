#include <EasyVulkan/CommandBuffer.hpp>
#include <EasyVulkan/CommandPool.hpp>
#include "CommandBuffer.hpp"

EasyVK::CommandBuffer::CommandBuffer(LogicalDevice *device, CommandPool *pool)
{
    _device = device;
    _pool = pool->getPool();

    VkCommandBufferAllocateInfo _allocInfo{};
    _allocInfo.sType =  VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    _allocInfo.commandPool = pool->getPool();
    _allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    _allocInfo.commandBufferCount = 1;

    _result = vkAllocateCommandBuffers(_device->getDevice(), &_allocInfo, &_buffer);
    _ready = _result == VK_SUCCESS;
}

EasyVK::CommandBuffer::CommandBuffer()
{
}

VkCommandBuffer EasyVK::CommandBuffer::getBuffer()
{
    return _buffer;
}

VkResult EasyVK::CommandBuffer::getResult()
{
    return _result;
}

bool EasyVK::CommandBuffer::isReady()
{
    return _ready;
}

void EasyVK::CommandBuffer::destroy()
{
    if(_ready){
        vkFreeCommandBuffers(_device->getDevice(), _pool, 1, &_buffer);
        _ready = false;
        _result = VK_NOT_READY;
    }
}

VkResult EasyVK::CommandBuffer::begin(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo BI{};
    BI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BI.flags = flags;
    return vkBeginCommandBuffer(_buffer, &BI);
}

void EasyVK::CommandBuffer::beginRenderPass(RenderPass renderPass, Framebuffer framebuffer, VkRect2D renderArea, std::vector<VkClearValue> clearColors)
{
    VkRenderPassBeginInfo RBI{};
    RBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    RBI.framebuffer = framebuffer.getFramebuffer();
    RBI.renderArea = renderArea;
    RBI.renderPass = renderPass.getRenderPass();
    RBI.pClearValues = clearColors.data();
    RBI.clearValueCount = clearColors.size();

    vkCmdBeginRenderPass(_buffer, &RBI, VK_SUBPASS_CONTENTS_INLINE_AND_SECONDARY_COMMAND_BUFFERS_EXT);
}

void EasyVK::CommandBuffer::endRenderPass()
{
    vkCmdEndRenderPass(_buffer);
}

void EasyVK::CommandBuffer::bindGraphicsPipeline(GraphicsPipeline pipeline)
{
    vkCmdBindPipeline(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());
}

void EasyVK::CommandBuffer::bindGraphicsDescriptorSet(GraphicsPipeline pipeline, VkDescriptorSet set)
{
    vkCmdBindDescriptorSets(_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, &set, 0, nullptr);
}

void EasyVK::CommandBuffer::setViewports(std::vector<VkViewport> viewports)
{
    vkCmdSetViewport(_buffer, 0, viewports.size(), viewports.data());
}

void EasyVK::CommandBuffer::setScissors(std::vector<VkRect2D> scissors)
{
    vkCmdSetScissor(_buffer, 0, scissors.size(), scissors.data());
}

void EasyVK::CommandBuffer::bindBuffers(std::vector<Buffer> buffers, std::vector<VkDeviceSize> offsets)
{
    std::vector<VkBuffer> buf;
    for(auto elem : buffers){
        buf.push_back(elem.getBuffer());
    }
    vkCmdBindVertexBuffers(_buffer, 0, buffers.size(), buf.data(), offsets.data());
}

void EasyVK::CommandBuffer::bindIndexBuffer(Buffer buffer, VkIndexType type)
{
    vkCmdBindIndexBuffer(_buffer, buffer.getBuffer(), 0, type);
}

void EasyVK::CommandBuffer::reset()
{
    vkResetCommandBuffer(_buffer, 0);
}

void EasyVK::CommandBuffer::copyBuffer(Buffer src, Buffer dst, size_t size)
{
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(_buffer, src.getBuffer(), dst.getBuffer(), 1, &copyRegion);
}

void EasyVK::CommandBuffer::copyBufferToImage(Buffer src, Image dst)
{
    VkBufferImageCopy copyRegion{};
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = 0;
    copyRegion.bufferImageHeight = 0;

    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.mipLevel = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageSubresource.baseArrayLayer = 0;

    copyRegion.imageExtent = dst.getExtent();
    copyRegion.imageOffset = {0,0,0};

    vkCmdCopyBufferToImage(_buffer, src.getBuffer(), dst.getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
}

void EasyVK::CommandBuffer::imageLayoutTransition(Image img, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier memBarier{};
    memBarier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memBarier.oldLayout = oldLayout;
    memBarier.newLayout = newLayout;

    memBarier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    memBarier.image = img.getImage();
    memBarier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memBarier.subresourceRange.baseArrayLayer = 0;
    memBarier.subresourceRange.layerCount = 1;
    memBarier.subresourceRange.baseMipLevel = 0;
    memBarier.subresourceRange.levelCount = 1;

    VkPipelineStageFlags srcFlags;
    VkPipelineStageFlags dstFlags;

    if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        memBarier.srcAccessMask = 0;
        memBarier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
        memBarier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memBarier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }else{
        return;
    }

    vkCmdPipelineBarrier(_buffer, srcFlags, dstFlags, 0, 0, nullptr, 0, nullptr, 1, &memBarier);
}

void EasyVK::CommandBuffer::draw(int count, int instances)
{
    vkCmdDraw(_buffer, count, instances, 0, 0);
}

void EasyVK::CommandBuffer::drawIndexed(uint32_t count, uint32_t instances)
{
    vkCmdDrawIndexed(_buffer, count, instances, 0, 0, 0);
}

VkResult EasyVK::CommandBuffer::end()
{
    return vkEndCommandBuffer(_buffer);
}
