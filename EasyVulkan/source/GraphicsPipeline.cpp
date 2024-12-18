//
// Created by Роман  Тимофеев on 24.04.2024.
//
#include <EasyVulkan/GraphicsPipeline.hpp>
#include <array>
#include <utility>

std::pair<std::vector<vk::VertexInputBindingDescription>, std::vector<vk::VertexInputAttributeDescription>>
EasyVK::GraphicsPipeline::unwrapVertexBinding(const std::vector<VertexBufferBinding>& vertexBindings) {

    std::vector<vk::VertexInputAttributeDescription> attributeDescription;
    std::vector<vk::VertexInputBindingDescription> bindingDescription;

    for(const auto& binding : vertexBindings){
        vk::VertexInputBindingDescription inputBindingDescription = {};
        inputBindingDescription.binding = binding.binding;
        inputBindingDescription.inputRate = binding.instanceData ? vk::VertexInputRate::eInstance : vk::VertexInputRate::eVertex;
        inputBindingDescription.stride = binding.stride;

        for(const auto& attribute : binding.attributes){
            vk::VertexInputAttributeDescription inputAttributeDescription = {};
            inputAttributeDescription.binding = binding.binding;
            inputAttributeDescription.offset = attribute.offset;
            inputAttributeDescription.location = attribute.location;

            switch (attribute.type) {
                case VertexBufferBinding::scalarFloat:
                    inputAttributeDescription.format = vk::Format::eR32Sfloat;
                    break;
                case VertexBufferBinding::scalarInteger:
                    inputAttributeDescription.format = vk::Format::eR32Sint;
                    break;
                case VertexBufferBinding::vec2f:
                    inputAttributeDescription.format = vk::Format::eR32G32Sfloat;
                    break;
                case VertexBufferBinding::vec3f:
                    inputAttributeDescription.format = vk::Format::eR32G32B32Sfloat;
                    break;
                case VertexBufferBinding::vec2i:
                    inputAttributeDescription.format = vk::Format::eR32G32Sint;
                    break;
                case VertexBufferBinding::vec3i:
                    inputAttributeDescription.format = vk::Format::eR32G32B32Sint;
                    break;

                default:
                    break;
            }

            attributeDescription.push_back(inputAttributeDescription);
        }

        bindingDescription.push_back(inputBindingDescription);
    }

    return {bindingDescription, attributeDescription};
}

void EasyVK::GraphicsPipeline::setup(uint32_t attachmentCount, vk::Device device, vk::RenderPass renderPass,
                                     EasyVK::ResourceDescriptor* resourceDescriptor,
                                     std::pair<Shader*, std::string> vertexShader,
                                     std::pair<Shader*, std::string> fragmentShader,
                                     const std::vector<VertexBufferBinding>& bufferBinding, vk::PrimitiveTopology topologyType,
                                     vk::CompareOp depthTestCompareOp, bool counterClockwiseTriangles) {

    vk::PipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
    vertexShaderCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    vertexShaderCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertexShaderCreateInfo.module = vertexShader.first->module;
    vertexShaderCreateInfo.pName = vertexShader.second.c_str();

    vk::PipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
    fragmentShaderCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    fragmentShaderCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragmentShaderCreateInfo.module = fragmentShader.first->module;
    fragmentShaderCreateInfo.pName = fragmentShader.second.c_str();

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vertexShaderCreateInfo, fragmentShaderCreateInfo};

    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    auto bufferBindings = unwrapVertexBinding(std::move(bufferBinding));
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = bufferBindings.first.size();
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = bufferBindings.first.data();
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = bufferBindings.second.size();
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = bufferBindings.second.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.topology = topologyType;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = vk::False;

    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
    pipelineRasterizationStateCreateInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    pipelineRasterizationStateCreateInfo.depthClampEnable = vk::False;
    pipelineRasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    pipelineRasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
    pipelineRasterizationStateCreateInfo.frontFace = counterClockwiseTriangles ? vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
    pipelineMultisampleStateCreateInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;

    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
    pipelineDepthStencilStateCreateInfo.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
    pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestCompareOp != vk::CompareOp::eNever;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthTestCompareOp != vk::CompareOp::eNever;
    pipelineDepthStencilStateCreateInfo.depthCompareOp = depthTestCompareOp;
    pipelineDepthStencilStateCreateInfo.stencilTestEnable = vk::False;
    pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
    pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
    pipelineColorBlendStateCreateInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    pipelineColorBlendStateCreateInfo.logicOpEnable = vk::False;
    std::vector<vk::PipelineColorBlendAttachmentState> attachmentStates = {};

    for(uint32_t i = 0; i < attachmentCount; i++){
        vk::PipelineColorBlendAttachmentState attachmentState = {};
        attachmentState.blendEnable = vk::False;
        attachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        attachmentStates.push_back(attachmentState);
    }

    pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
    pipelineColorBlendStateCreateInfo.pAttachments = attachmentStates.data();

    vk::PipelineDynamicStateCreateInfo stateCreateInfo = {};
    stateCreateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    std::array<vk::DynamicState, 2> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    stateCreateInfo.pDynamicStates = dynamicStates.data();
    stateCreateInfo.dynamicStateCount = 2;

    vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    layoutCreateInfo.setLayoutCount = 1;
    layoutCreateInfo.pSetLayouts = &(resourceDescriptor->descriptorLayout);

    this->layout = device.createPipelineLayout(layoutCreateInfo);
    this->device = device;

    vk::Viewport viewport = vk::Viewport(0,0,0,0);
    vk::Rect2D scissor = vk::Rect2D({0,0}, {0,0});

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    pipelineCreateInfo.layout = this->layout;
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &stateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    this->pipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineCreateInfo).value;
}

EasyVK::GraphicsPipeline::~GraphicsPipeline() {
    this->device.destroy(this->pipeline);
    this->device.destroy(this->layout);
}
