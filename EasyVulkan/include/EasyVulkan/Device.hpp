//
// Created by Роман  Тимофеев on 20.04.2024.
//

#ifndef EASY_VULKAN_TEST_DEVICE_HPP
#define EASY_VULKAN_TEST_DEVICE_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/Base.hpp>
#include <EasyVulkan/DeviceFeatures.hpp>
#include <EasyVulkan/CommandBuffer.hpp>
#include <EasyVulkan/Buffer.hpp>
#include <EasyVulkan/Image.hpp>
#include <EasyVulkan/Shader.hpp>
#include <EasyVulkan/ResourceDescriptor.hpp>
#include <EasyVulkan/ComputePipeline.hpp>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/SwapChain.hpp>
#include <unordered_map>
#include <vector>
#include <cctype>

namespace EasyVK {
    class PhysicalDevice;

    class Device {
    public:
        enum BufferAccessFlagBits : uint32_t {
            Graphics = 0b1,
            Transfer = 0b10,
            Compute = 0b100,
            Host = 0b1000,
            Present = 0b10000
        };

        typedef uint32_t BufferAccessFlags;

    public:
        ~Device();

    private:
        std::pair<uint32_t, std::vector<vk::Queue>> compute;
        std::pair<uint32_t, std::vector<vk::Queue>> graphics;
        std::pair<uint32_t, std::vector<vk::Queue>> transfer;
        std::pair<uint32_t, std::vector<vk::Queue>> presentQueue;
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
        std::unordered_map<uint32_t, vk::CommandPool> commandPools;
        Features enabledFeatures = 0x0;
        bool set_up = false;

    private:
        CommandBuffer* createCommandBuffer(uint32_t family, vk::Queue queue, Features requiredFeatures);

    protected:
        void setUpWithDesired(vk::Instance instance, Features features, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);

    public:
        uint32_t getQueueCountFor(BufferAccessFlagBits flagBits);
        CommandBuffer* createGraphicsCommandBuffer(uint32_t queueId);
        CommandBuffer* createComputeCommandBuffer(uint32_t queueId);
        CommandBuffer* createTransferCommandBuffer(uint32_t queueId);
        Buffer* createBuffer(size_t size, BufferAccessFlags accessFlags);
        Image* createImage(vk::Extent3D size, vk::Format colorFormat, BufferAccessFlags accessFlags);
        ResourceDescriptor* createResourceDescriptor(const std::map<uint32_t, ResourceDescriptor::ResourceBindingDescriptor> &bindings);
        Shader* createShader(const std::vector<uint32_t>& shaderCode);
        ComputePipeline* createComputePipeline(std::pair<Shader*, std::string> shader, ResourceDescriptor* descriptor);
        RenderPass* createRenderPass(const std::vector<std::pair<Image*, RenderPass::ImageOp>>& images);
        SwapChain* createSwapChain(vk::SurfaceKHR surfaceKhr, uint32_t queueId);

    private:
        friend PhysicalDevice;
    };
}

#endif //EASY_VULKAN_TEST_DEVICE_HPP
