#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/PhysicalDevice.hpp>
#include <EasyVulkan/QueueFamilies.hpp>
#include <EasyVulkan/Queue.hpp>
#include <EasyVulkan/Framebuffer.hpp>
#include <EasyVulkan/Shader.hpp>
#include <EasyVulkan/GraphicsPipeline.hpp>
#include <EasyVulkan/Semaphore.hpp>
#include <EasyVulkan/Fence.hpp>
#include <EasyVulkan/Buffer.hpp>
#include <EasyVulkan/DescriptorPool.hpp>
#include <EasyVulkan/Image.hpp>
#include <vector>

namespace EasyVK{

    class Queue;
    class SwapChain;

    class LogicalDevice{
    private: 
        struct QueueSetting{
            unsigned int count = 0;
            QueueThat flags;
            std::vector<float> priorieties = {};
            bool canPresent = false;
        };
    
    private:
        PhysicalDevice _my_device;
        VkDevice _device = VK_NULL_HANDLE;
        VkResult _result = VK_NOT_READY;
        bool _created = false;
        std::vector<Queue> _savedQueues;

        LogicalDevice(const PhysicalDevice& device);
        LogicalDevice(const PhysicalDevice& device, const std::vector<QueueProps>& queues);

    public:
        LogicalDevice();

        void init(
            const std::vector<QueueProps>& queues
        );

        std::vector<VkSurfaceFormatKHR> getSupportedFormats();
        std::vector<VkPresentModeKHR> getSupportedPresentModes();
        VkSurfaceCapabilitiesKHR getCapabilities();

        VkDevice getDevice();
        PhysicalDevice getPhysicalDevice();
        Shader createShader(std::vector<char> source);
        Queue getQueue(QueueThat that);
        Queue getPresentQueue();
        std::vector<Queue> getAllQueues();
        
        VkResult getResult();
        Framebuffer createFrameBuffer(std::vector<EasyVK::ImageView> views, VkExtent2D extent, RenderPass renderPass, uint32_t layers);
        bool ready();

        SwapChain createSwapChain(
            unsigned int frameWidth,
            unsigned int frameHeight,
            bool depth_test = false,
            VkSurfaceFormatKHR prefered_format = {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            VkPresentModeKHR prefered_presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            uint32_t imageArrayLayers = 1,
            VkImageUsageFlags flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VkBool32 clipped = VK_TRUE
        );

        GraphicsPipeline createGraphicsPipeline(
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages,
            std::vector<VkDynamicState> dynamicStates,
            std::vector<VkVertexInputBindingDescription> inputBindings,
            std::vector<VkVertexInputAttributeDescription> inputAttributes,
            std::vector<VkViewport> viewports,
            std::vector<VkRect2D> scissors,
            VkCullModeFlags cullMode,
            VkFrontFace fronFace,
            RenderPass renderPass,
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments,
            std::vector<DescriptorLayout> descriptors = {},
            bool depthTestEnable = false,
            bool depthWriteEnable = false,
            VkCompareOp depthCmpMode = VK_COMPARE_OP_LESS,
            VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
            VkPrimitiveTopology primitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
        );

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags);

        Semaphore createSemaphore();
        Fence createFence(bool startSignaled);
        Buffer createBuffer(
            uint64_t size,
            bool deviceAccesible,
            VkBufferUsageFlags usage
        );

        Image createImage(
            VkExtent3D extent, 
            VkFormat format, 
            bool cpuAccess,
            VkImageUsageFlags usage, 
            VkImageType type = VK_IMAGE_TYPE_2D
        );
        
        DescriptorPool createDescriptorPool(
            uint32_t maxSize, 
            std::vector<VkDescriptorType> types, 
            std::vector<uint32_t> count = {}
        );

        void waitIdle();

        void destroy();

    friend PhysicalDevice;    
    };
}