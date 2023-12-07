#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/DescriptorLayout.hpp>
#include <vector>

namespace EasyVK{

    class LogicalDevice;

    class GraphicsPipeline{
    
    private:
        VkPipeline _pipeline;
        VkPipelineLayout _pipelineLayout;
        VkDescriptorSetLayout _descriptorLayout;
        LogicalDevice* _device;


        VkResult _result = VK_NOT_READY;
        bool _ready = false;

        GraphicsPipeline(LogicalDevice* logicalDevice);

    public:

        GraphicsPipeline();

        void init(
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages,
            std::vector<VkDynamicState> dynamicStates,
            std::vector<VkVertexInputBindingDescription> inputBindings,
            std::vector<VkVertexInputAttributeDescription> inputAttributes,
            std::vector<VkViewport> viewports,
            std::vector<VkRect2D> scissors,
            VkCullModeFlags cullMode,
            VkFrontFace frontFace,
            RenderPass renderPass,
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments,
            std::vector<DescriptorLayout> descriptors = {},
            bool depthTestEnable = false,
            bool depthWriteEnable = false,
            VkCompareOp depthCmpMode = VK_COMPARE_OP_LESS,
            VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
            VkPrimitiveTopology primitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
        );

        VkPipeline getPipeline();
        VkPipelineLayout getPipelineLayout();
        VkResult getResult();
        VkDescriptorSetLayout getDescriptorLayout();
        bool getReady();

        void destroy();
    friend LogicalDevice;
    };
}