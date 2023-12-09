#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <string>

namespace EasyVK{

    class LogicalDevice;

    class Shader{
    
    private:
        LogicalDevice* _device;
        bool _ready = false;
        VkResult _result = VK_NOT_READY;
        VkShaderModule _module; 

        Shader(LogicalDevice *device);

    public:
        Shader();

        void init(std::vector<char> source);

        bool isReady();
        VkResult getResult();
        VkShaderModule getModule();
        VkPipelineShaderStageCreateInfo getFragmentStage(const char* name);
        VkPipelineShaderStageCreateInfo getVertexStage(const char* name);

        void destroy();
        
    friend LogicalDevice;
    };
}