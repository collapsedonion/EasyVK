//
// Created by Роман  Тимофеев on 22.04.2024.
//
#include <EasyVulkan/Shader.hpp>

void EasyVK::Shader::setup(vk::Device device, const std::vector<uint32_t>& data) {
    vk::ShaderModuleCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    createInfo.codeSize = data.size();
    createInfo.pCode = data.data();

    this->device = device;
    this->module = device.createShaderModule(createInfo);
}

EasyVK::Shader::~Shader() {
    this->device.destroy(this->module);
}
