#pragma once
#include <vector>
#include <vulkan/vulkan.h>

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
        VkPipelineShaderStageCreateInfo getFragmentStage(std::string name);
        VkPipelineShaderStageCreateInfo getVertexStage(std::string name);

        void destroy();
        
    friend LogicalDevice;
    };
}