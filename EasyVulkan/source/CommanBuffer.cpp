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
    vk::SemaphoreCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    this->mySemaphore = _device.createSemaphore(createInfo);

    vk::FenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceCreateInfo.flags |= vk::FenceCreateFlagBits::eSignaled;
    this->fence = _device.createFence(fenceCreateInfo);

    this->device = _device;
    this->myCommandPool = commandPool;
}

EasyVK::CommandBuffer::~CommandBuffer() {
    if(isKilled()) {
        this->device.destroy(fence);
        this->device.destroy(mySemaphore);
        this->device.freeCommandBuffers(this->myCommandPool, {this->bufferHandler});
    }
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

void EasyVK::CommandBuffer::submit(const std::vector<CommandBuffer>& waitCommandBuffers) {
    vk::SubmitInfo submitInfo = {};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->bufferHandler;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &this->mySemaphore;

    device.resetFences({this->fence});

    std::vector<vk::Semaphore> waitSemaphores;
    waitSemaphores.resize(waitCommandBuffers.size());

    for(const auto& commandBuffer : waitCommandBuffers){
        waitSemaphores.push_back(commandBuffer.mySemaphore);
    }

    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();

    this->submitQueue.submit(submitInfo, fence);
}

void EasyVK::CommandBuffer::bind(EasyVK::ComputePipeline pipeline) {
    this->bufferHandler.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.pipeline);
}

void EasyVK::CommandBuffer::bind(EasyVK::ComputePipeline pipeline, EasyVK::ResourceDescriptor descriptor) {
    this->bufferHandler.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline.layout, 0, 1, &descriptor.descriptorSet, 0,
                                           nullptr);
}

void EasyVK::CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    this->bufferHandler.dispatch(groupCountX, groupCountY, groupCountZ);
}

void EasyVK::CommandBuffer::await() {
    this->device.waitForFences({fence}, true, UINT32_MAX);
}

void EasyVK::CommandBuffer::beginRenderPass(EasyVK::RenderPass renderPass, uint32_t frameBufferIndex) {
    if(renderPass.frameBuffer.size() < frameBufferIndex){
        throw std::runtime_error("Framebuffer index out of bounds");
    }

    vk::RenderPassBeginInfo beginInfo = {};
    beginInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    beginInfo.renderArea = renderPass.renderArea[renderPass.frameBuffer[frameBufferIndex]];
    beginInfo.renderPass = renderPass.renderPass;
    beginInfo.clearValueCount = renderPass.values.size();
    beginInfo.pClearValues = renderPass.values.data();
    beginInfo.framebuffer = renderPass.frameBuffer[frameBufferIndex];

    this->bufferHandler.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
}

void EasyVK::CommandBuffer::endRenderPass() {
    this->bufferHandler.endRenderPass();
}

void EasyVK::CommandBuffer::bind(EasyVK::GraphicsPipeline graphicsPipeline) {
    this->bufferHandler.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.pipeline);
}

void EasyVK::CommandBuffer::bind(EasyVK::GraphicsPipeline graphicsPipeline, EasyVK::ResourceDescriptor descriptor) {
    this->bufferHandler.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipeline.layout, 0, 1, &descriptor.descriptorSet, 0,
                                           nullptr);
}

void EasyVK::CommandBuffer::draw(uint32_t count, uint32_t instances) {
    this->bufferHandler.draw(count, instances, 0, 0);
}

void EasyVK::CommandBuffer::setScissors(std::vector<vk::Rect2D> scissors) {
    this->bufferHandler.setScissor(0, scissors);
}

void EasyVK::CommandBuffer::setViewport(std::vector<vk::Viewport> viewports) {
    this->bufferHandler.setViewport(0, viewports);
}

void EasyVK::CommandBuffer::bindVertexBuffer(EasyVK::Buffer buffer) {
    this->bufferHandler.bindVertexBuffers(0, {buffer.buffer}, {0});
}
