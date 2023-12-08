#include <EasyVulkan/Shader.hpp>
#include <EasyVulkan/LogicalDevice.hpp>
#include "Shader.hpp"
#include <string>

EasyVK::Shader::Shader(LogicalDevice *device)
{
    _device = device;
}

EasyVK::Shader::Shader()
{
}

void EasyVK::Shader::init(std::vector<char> source)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType =  VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = (uint32_t*)source.data();
    createInfo.codeSize = source.size();
    
    _result = vkCreateShaderModule(_device->getDevice(), &createInfo, nullptr, &_module);
    _ready = _result == VK_SUCCESS;
}

bool EasyVK::Shader::isReady()
{
    return _ready;
}

VkResult EasyVK::Shader::getResult()
{
    return _result;
}

VkShaderModule EasyVK::Shader::getModule()
{
    return _module;
}

VkPipelineShaderStageCreateInfo EasyVK::Shader::getFragmentStage(std::string name)
{
    VkPipelineShaderStageCreateInfo ci{};
    ci.module = _module;
    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ci.pName = name.data();
    ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    return ci;
}

VkPipelineShaderStageCreateInfo EasyVK::Shader::getVertexStage(std::string name)
{
    VkPipelineShaderStageCreateInfo ci{};
    ci.module = _module;
    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ci.pName = name.data();
    ci.stage = VK_SHADER_STAGE_VERTEX_BIT;

    return ci;
}

void EasyVK::Shader::destroy()
{
    if(_ready){
        _ready = false;
        _result = VK_NOT_READY;
        vkDestroyShaderModule(_device->getDevice(), _module, nullptr);
    }
}
