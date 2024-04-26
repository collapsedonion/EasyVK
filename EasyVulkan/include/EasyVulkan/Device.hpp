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
#include <map>
#include <vector>
#include <cctype>

namespace EasyVK {
    class PhysicalDevice;

    class Device : AutoFree{
    public:
        enum BufferAccessFlagBits : uint32_t {
            Graphics,
            Transfer,
            Compute,
            Host
        };

        typedef uint32_t BufferAccessFlags;

    public:
        ~Device();

    private:
        std::pair<uint32_t, vk::Queue> compute;
        std::pair<uint32_t, vk::Queue> graphics;
        std::pair<uint32_t, vk::Queue> transfer;
        std::pair<uint32_t, vk::Queue> presentQueue;
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
        std::map<uint32_t, vk::CommandPool> commandPools;
        Features enabledFeatures = 0x0;
        bool set_up = false;

    private:
        CommandBuffer createCommandBuffer(uint32_t family, vk::Queue queue, Features requiredFeatures);

    protected:
        void setUpWithDesired(vk::Instance instance, Features features, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);

    public:
        CommandBuffer createGraphicsCommandBuffer();
        CommandBuffer createComputeCommandBuffer();
        CommandBuffer createTransferCommandBuffer();
        Buffer createBuffer(size_t size, BufferAccessFlags accessFlags);
        Image createImage(vk::Extent3D size, vk::Format colorFormat, BufferAccessFlags accessFlags);
        ResourceDescriptor createResourceDescriptor(const std::map<uint32_t, ResourceDescriptor::ResourceBindingDescriptor> &bindings);
        Shader createShader(const std::vector<uint32_t>& shaderCode);
        ComputePipeline createComputePipeline(std::pair<Shader, std::string> shader, ResourceDescriptor descriptor);
        RenderPass createRenderPass(const std::vector<Image> images);
        SwapChain createSwapChain(vk::SurfaceKHR surfaceKhr);

    private:
        friend PhysicalDevice;
    };
}

#endif //EASY_VULKAN_TEST_DEVICE_HPP
