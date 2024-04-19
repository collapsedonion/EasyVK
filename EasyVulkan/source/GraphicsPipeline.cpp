#include <EasyVulkan/GraphicsPipeline.hpp>
#include <EasyVulkan/LogicalDevice.hpp>

EasyVK::GraphicsPipeline::GraphicsPipeline(LogicalDevice *logicalDevice)
{
    _device = logicalDevice;
}

EasyVK::GraphicsPipeline::GraphicsPipeline()
{
}

void EasyVK::GraphicsPipeline::init(
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
    std::vector<DescriptorLayout> descriptors,
    bool depthTestEnable,
    bool depthWriteEnable,
    VkCompareOp depthCmpMode,
    VkPolygonMode polygonMode,
    VkPrimitiveTopology primitive)
{
    VkPipelineDynamicStateCreateInfo plDynamicStateCI{};
    plDynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    plDynamicStateCI.dynamicStateCount = dynamicStates.size();
    plDynamicStateCI.pDynamicStates = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo plVertexInputStateCI{};
    plVertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    plVertexInputStateCI.vertexAttributeDescriptionCount = inputAttributes.size();
    plVertexInputStateCI.pVertexAttributeDescriptions = inputAttributes.data();
    plVertexInputStateCI.vertexBindingDescriptionCount = inputBindings.size();
    plVertexInputStateCI.pVertexBindingDescriptions = inputBindings.data();

    VkPipelineInputAssemblyStateCreateInfo plInputAssemblyStateCI{};
    plInputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    plInputAssemblyStateCI.topology = primitive;
    plInputAssemblyStateCI.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo plViewportStateCI{};
    plViewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    plViewportStateCI.pScissors = scissors.data();
    plViewportStateCI.pViewports = viewports.data();
    plViewportStateCI.viewportCount = viewports.size();
    plViewportStateCI.scissorCount = scissors.size();

    VkPipelineRasterizationStateCreateInfo plRasterizationCI{};
    plRasterizationCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    plRasterizationCI.depthClampEnable = VK_FALSE;
    plRasterizationCI.rasterizerDiscardEnable = VK_FALSE;
    plRasterizationCI.polygonMode = polygonMode;
    plRasterizationCI.lineWidth = 1.0f;
    plRasterizationCI.cullMode = cullMode;
    plRasterizationCI.frontFace = frontFace;
    plRasterizationCI.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo plMultisampleCI{};
    plMultisampleCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    plMultisampleCI.sampleShadingEnable = VK_FALSE;
    plMultisampleCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendStateCreateInfo plColorBlendCI{};
    plColorBlendCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    plColorBlendCI.logicOpEnable = VK_FALSE;
    plColorBlendCI.attachmentCount = colorBlendAttachments.size();
    plColorBlendCI.pAttachments = colorBlendAttachments.data();

    VkPipelineLayoutCreateInfo plLayoutCI{};
    plLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for(auto elem : descriptors){
        bindings.push_back(elem.getBinding());
    }

    VkDescriptorSetLayoutCreateInfo dSetLayoutCI{};
    dSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dSetLayoutCI.bindingCount = bindings.size();
    dSetLayoutCI.pBindings = bindings.data();

    _result = vkCreateDescriptorSetLayout(_device->getDevice(), &dSetLayoutCI, nullptr, &_descriptorLayout);
     _ready = _result == VK_SUCCESS;
    if(!_ready){
        return;
    }

    plLayoutCI.setLayoutCount = 1;
    plLayoutCI.pSetLayouts = &_descriptorLayout;

    VkPipelineDepthStencilStateCreateInfo plDepthStencilCI{};
    plDepthStencilCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    plDepthStencilCI.stencilTestEnable = VK_FALSE;
    plDepthStencilCI.depthTestEnable = depthTestEnable;
    plDepthStencilCI.depthWriteEnable = depthWriteEnable;
    plDepthStencilCI.depthCompareOp = depthCmpMode;

    _result = vkCreatePipelineLayout(_device->getDevice(), &plLayoutCI, nullptr, &_pipelineLayout);
    _ready = _result == VK_SUCCESS;
    if(!_ready){
        return;
    }

    VkGraphicsPipelineCreateInfo gplCI{};
    gplCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gplCI.layout = _pipelineLayout;
    gplCI.stageCount = shaderStages.size();
    gplCI.pStages = shaderStages.data();
    gplCI.pVertexInputState = &plVertexInputStateCI;
    gplCI.pColorBlendState = &plColorBlendCI;
    gplCI.pDepthStencilState = &plDepthStencilCI;
    gplCI.pDynamicState = &plDynamicStateCI;
    gplCI.pInputAssemblyState = &plInputAssemblyStateCI;
    gplCI.pMultisampleState = &plMultisampleCI;
    gplCI.pRasterizationState = &plRasterizationCI;
    gplCI.pViewportState = &plViewportStateCI;
    gplCI.renderPass = renderPass.getRenderPass();
    gplCI.subpass = 0;

    _result = vkCreateGraphicsPipelines(_device->getDevice(), VK_NULL_HANDLE, 1, &gplCI, nullptr, &_pipeline);
    _ready = _result == VK_SUCCESS;

    if(!_ready){
        vkDestroyPipelineLayout(_device->getDevice(), _pipelineLayout, nullptr);
    }
}

VkPipeline EasyVK::GraphicsPipeline::getPipeline()
{
    return _pipeline;
}

VkPipelineLayout EasyVK::GraphicsPipeline::getPipelineLayout()
{
    return _pipelineLayout;
}

VkResult EasyVK::GraphicsPipeline::getResult()
{
    return _result;
}

VkDescriptorSetLayout EasyVK::GraphicsPipeline::getDescriptorLayout()
{
    return _descriptorLayout;
}

bool EasyVK::GraphicsPipeline::getReady()
{
    return _ready;
}

void EasyVK::GraphicsPipeline::destroy()
{
    if(_ready){
        _ready = false;
        _result = VK_NOT_READY;
        vkDestroyPipeline(_device->getDevice(), _pipeline, nullptr);
        vkDestroyPipelineLayout(_device->getDevice(), _pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(_device->getDevice(), _descriptorLayout, nullptr);
    }
}
