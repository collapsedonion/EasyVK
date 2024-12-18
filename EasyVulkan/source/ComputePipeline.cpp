//
// Created by Роман  Тимофеев on 23.04.2024.
//
#include <EasyVulkan/ComputePipeline.hpp>

void
EasyVK::ComputePipeline::setup(std::pair<Shader*, std::string> shader, EasyVK::ResourceDescriptor* resourceDescriptor,
                               vk::Device device) {

    vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    layoutCreateInfo.setLayoutCount = 1;
    layoutCreateInfo.pSetLayouts = &(resourceDescriptor->descriptorLayout);

    this->device = device;
    this->layout = device.createPipelineLayout(layoutCreateInfo);

    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    shaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eCompute;
    shaderStageCreateInfo.pName = shader.second.data();
    shaderStageCreateInfo.module = shader.first->module;

    vk::ComputePipelineCreateInfo computePipelineCreateInfo = {};
    computePipelineCreateInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
    computePipelineCreateInfo.layout = this->layout;
    computePipelineCreateInfo.stage = shaderStageCreateInfo;

    pipeline = device.createComputePipeline(VK_NULL_HANDLE, computePipelineCreateInfo).value;
}

EasyVK::ComputePipeline::~ComputePipeline() {
    this->device.destroy(this->layout);
    this->device.destroy(this->pipeline);  
}
