//
// Created by Роман  Тимофеев on 21.04.2024.
//
#include <EasyVulkan/CommandBuffer.hpp>

void EasyVK::CommandBuffer::allocateCommandBuffer(vk::CommandPool commandPool, vk::Queue _submitQueue, vk::Device _device){
    this->submitQueue = _submitQueue;

    vk::CommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    allocateInfo.commandPool = commandPool;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = 1;

    this->bufferHandler = _device.allocateCommandBuffers(allocateInfo)[0];

    vk::FenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceCreateInfo.flags |= vk::FenceCreateFlagBits::eSignaled;
    this->fence = _device.createFence(fenceCreateInfo);

    this->device = _device;
    this->myCommandPool = commandPool;
}

EasyVK::CommandBuffer::~CommandBuffer() {
    this->device.destroy(fence);
    this->device.freeCommandBuffers(this->myCommandPool, {this->bufferHandler});    
}

void EasyVK::CommandBuffer::reset() {
    bufferHandler.reset();
}

void EasyVK::CommandBuffer::begin() {
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    this->bufferHandler.begin(beginInfo);
}

void EasyVK::CommandBuffer::end() {
    this->bufferHandler.end();
}

void EasyVK::CommandBuffer::submit() {
    vk::SubmitInfo submitInfo = {};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->bufferHandler;
    submitInfo.signalSemaphoreCount = 0;
    
    device.resetFences({this->fence});

    submitInfo.waitSemaphoreCount = 0;

    this->submitQueue.submit(submitInfo, fence);
}

void EasyVK::CommandBuffer::bind(EasyVK::ComputePipeline* pipeline) {
    this->bufferHandler.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline->pipeline);
}

void EasyVK::CommandBuffer::bind(EasyVK::ComputePipeline* pipeline, EasyVK::ResourceDescriptor* descriptor) {
    this->bufferHandler.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline->layout, 0, 1, &(descriptor->descriptorSet), 0,
                                           nullptr);
}

void EasyVK::CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    this->bufferHandler.dispatch(groupCountX, groupCountY, groupCountZ);
}

void EasyVK::CommandBuffer::await() {
    this->device.waitForFences({fence}, true, UINT32_MAX);
}

void EasyVK::CommandBuffer::transferImageLayout(EasyVK::Image *img, vk::ImageLayout targetLayout)
{
    auto fromLayout = img->layout;

    vk::ImageMemoryBarrier memoryBarier = {};

    if(fromLayout == vk::ImageLayout::eUndefined){
        memoryBarier.srcAccessMask = vk::AccessFlagBits::eNone;
    }else if(fromLayout == vk::ImageLayout::eColorAttachmentOptimal){
        memoryBarier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    }
    else if(fromLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal){
        memoryBarier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    }else if(fromLayout == vk::ImageLayout::eTransferDstOptimal){
        memoryBarier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    }else if(fromLayout == vk::ImageLayout::ePresentSrcKHR){
        memoryBarier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead;
    }else if(fromLayout == vk::ImageLayout::eTransferSrcOptimal){
        memoryBarier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    }else if(fromLayout == vk::ImageLayout::eShaderReadOnlyOptimal){
        memoryBarier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    }

    if(targetLayout == vk::ImageLayout::eUndefined){
        memoryBarier.dstAccessMask = vk::AccessFlagBits::eNone;
    }else if(targetLayout == vk::ImageLayout::eColorAttachmentOptimal){
        memoryBarier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    }
    else if(targetLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal){
        memoryBarier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    }
    else if(targetLayout == vk::ImageLayout::eTransferDstOptimal){
        memoryBarier.dstAccessMask = vk::AccessFlagBits::eTransferRead | vk::AccessFlagBits::eTransferWrite;
    }else if(targetLayout == vk::ImageLayout::ePresentSrcKHR){
        memoryBarier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead;
    }else if(targetLayout == vk::ImageLayout::eTransferSrcOptimal){
        memoryBarier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    }else if(targetLayout == vk::ImageLayout::eShaderReadOnlyOptimal){
        memoryBarier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    }

    memoryBarier.oldLayout = fromLayout;
    memoryBarier.newLayout = targetLayout;
    memoryBarier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    if(img->colorFormat == vk::Format::eD32SfloatS8Uint ||
           img->colorFormat == vk::Format::eD32Sfloat ||
           img->colorFormat == vk::Format::eD24UnormS8Uint ||
           img->colorFormat == vk::Format::eD16UnormS8Uint ||
           img->colorFormat == vk::Format::eD16Unorm){
        memoryBarier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    }else{
        memoryBarier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    memoryBarier.subresourceRange.levelCount = 1;
    memoryBarier.subresourceRange.baseMipLevel = 0;
    memoryBarier.subresourceRange.baseArrayLayer = 0;
    memoryBarier.subresourceRange.layerCount = 1;

    memoryBarier.image = img->image;

    this->bufferHandler.pipelineBarrier(vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllGraphics, (vk::DependencyFlagBits)0, {}, {}, {memoryBarier});

    img->layout = targetLayout;
}

void EasyVK::CommandBuffer::clearDepthImage(EasyVK::Image *img, vk::ClearDepthStencilValue clearValue)
{
    vk::ImageSubresourceRange range = {};
    range.levelCount = 1;
    range.baseMipLevel = 0;
    range.baseArrayLayer = 0;
    range.layerCount = 1;
    range.aspectMask = vk::ImageAspectFlagBits::eDepth;

    this->bufferHandler.clearDepthStencilImage(img->image, img->layout, clearValue, {
        range        
    });
}

void EasyVK::CommandBuffer::clearColorImage(EasyVK::Image *img, vk::ClearColorValue clearValue)
{
    vk::ImageSubresourceRange range = {};
    range.levelCount = 1;
    range.baseMipLevel = 0;
    range.baseArrayLayer = 0;
    range.layerCount = 1;
    range.aspectMask = vk::ImageAspectFlagBits::eColor;

    this->bufferHandler.clearColorImage(img->image, img->layout, clearValue, {
        range        
    });
   
}

void EasyVK::CommandBuffer::beginRenderPass(EasyVK::RenderPass *renderPass, uint32_t frameBufferIndex)
{
    if(renderPass->frameBuffer.size() < frameBufferIndex){
        throw std::runtime_error("Framebuffer index out of bounds");
    }

    vk::RenderPassBeginInfo beginInfo = {};
    beginInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    beginInfo.renderArea = renderPass->renderArea[renderPass->frameBuffer[frameBufferIndex]];
    beginInfo.renderPass = renderPass->renderPass;
    beginInfo.clearValueCount = renderPass->values.size();
    beginInfo.pClearValues = renderPass->values.data();
    beginInfo.framebuffer = renderPass->frameBuffer[frameBufferIndex];

    this->bufferHandler.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
}

void EasyVK::CommandBuffer::endRenderPass() {
    this->bufferHandler.endRenderPass();
}

void EasyVK::CommandBuffer::copy(EasyVK::Buffer* src, EasyVK::Buffer* dst)
{
    this->copy(src, dst, std::min(src->allocatedSize, dst->allocatedSize));
}

void EasyVK::CommandBuffer::copy(EasyVK::Buffer* src, EasyVK::Buffer* dst, uint32_t size_to_copy)
{
    this->bufferHandler.copyBuffer(src->buffer, dst->buffer, {
        vk::BufferCopy(0, 0, size_to_copy)
    });
}

void EasyVK::CommandBuffer::copy(EasyVK::Image *src, EasyVK::Image *dst)
{
    if(src->colorFormat != dst->colorFormat){
        return;
    }

    uint32_t copyWidth = std::min(src->size.width, dst->size.width);
    uint32_t copyHeight = std::min(src->size.height, dst->size.width);

    vk::ImageCopy copyRegion;
    copyRegion.srcOffset.x = 0;
    copyRegion.srcOffset.y = 0;
    copyRegion.srcOffset.z = 0;

    copyRegion.dstOffset.x = 0;
    copyRegion.dstOffset.y = 0;
    copyRegion.dstOffset.z = 0;   

    copyRegion.extent.width = copyWidth;
    copyRegion.extent.height = copyHeight;
    copyRegion.extent.depth = 1;

    vk::ImageSubresourceLayers range = {};

    if(src->colorFormat == vk::Format::eD32SfloatS8Uint ||
        src->colorFormat == vk::Format::eD32Sfloat ||
        src->colorFormat == vk::Format::eD24UnormS8Uint ||
        src->colorFormat == vk::Format::eD16UnormS8Uint ||
        src->colorFormat == vk::Format::eD16Unorm){
        range.aspectMask = vk::ImageAspectFlagBits::eDepth;
    }else{
        range.aspectMask = vk::ImageAspectFlagBits::eColor;
    }
    range.mipLevel = 0;
    range.baseArrayLayer = 0;
    range.layerCount = 1;
    
    copyRegion.srcSubresource = range;
    copyRegion.dstSubresource = range;

    this->bufferHandler.copyImage(
        src->image,
        src->layout,
        dst->image,
        dst->layout,
        {copyRegion}
    );
}

void EasyVK::CommandBuffer::copy(EasyVK::Buffer *src, EasyVK::Image *dst)
{
    vk::BufferImageCopy copyInfo = {};
    copyInfo.bufferOffset = 0;
    copyInfo.bufferRowLength = 0;
    copyInfo.bufferImageHeight = 0;

    copyInfo.imageOffset.x = 0;
    copyInfo.imageOffset.y = 0;
    copyInfo.imageOffset.z = 0;   

    copyInfo.imageExtent.width = dst->size.width;
    copyInfo.imageExtent.height = dst->size.height;
    copyInfo.imageExtent.depth = 1;

    vk::ImageSubresourceLayers range = {};

    if(dst->colorFormat == vk::Format::eD32SfloatS8Uint ||
        dst->colorFormat == vk::Format::eD32Sfloat ||
        dst->colorFormat == vk::Format::eD24UnormS8Uint ||
        dst->colorFormat == vk::Format::eD16UnormS8Uint ||
        dst->colorFormat == vk::Format::eD16Unorm){
        range.aspectMask = vk::ImageAspectFlagBits::eDepth;
    }else{
        range.aspectMask = vk::ImageAspectFlagBits::eColor;
    }
    range.mipLevel = 0;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    copyInfo.imageSubresource = range;

    this->bufferHandler.copyBufferToImage(src->buffer, dst->image, vk::ImageLayout::eTransferDstOptimal, {copyInfo});    
}

void EasyVK::CommandBuffer::bind(EasyVK::GraphicsPipeline *graphicsPipeline)
{
    this->bufferHandler.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->pipeline);
}

void EasyVK::CommandBuffer::bind(EasyVK::GraphicsPipeline* graphicsPipeline, EasyVK::ResourceDescriptor* descriptor) {
    this->bufferHandler.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipeline->layout, 0, 1, &(descriptor->descriptorSet), 0,
                                           nullptr);
}

void EasyVK::CommandBuffer::draw(uint32_t count, uint32_t instances) {
    this->bufferHandler.draw(count, instances, 0, 0);
}

void EasyVK::CommandBuffer::drawIndexed(uint32_t indexCount)
{
    this->bufferHandler.drawIndexed(indexCount, 1, 0, 0, 0);
}

void EasyVK::CommandBuffer::setScissors(std::vector<vk::Rect2D> scissors)
{
    this->bufferHandler.setScissor(0, scissors);
}

void EasyVK::CommandBuffer::setViewport(std::vector<vk::Viewport> viewports) {
    this->bufferHandler.setViewport(0, viewports);
}

void EasyVK::CommandBuffer::bindVertexBuffer(EasyVK::Buffer* buffer) {
    this->bufferHandler.bindVertexBuffers(0, {buffer->buffer}, {0});
}

void EasyVK::CommandBuffer::bindIndexBuffer(EasyVK::Buffer* buffer, bool intIndicies)
{
    this->bufferHandler.bindIndexBuffer(buffer->buffer, 0, intIndicies ? vk::IndexType::eUint32 : vk::IndexType::eUint16);
}
