#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace EasyVK{

    class LogicalDevice;
    class SwapChain;

    class RenderPass{

    private:

        VkRenderPass _renderpass = VK_NULL_HANDLE;
        LogicalDevice* _device = nullptr;

        VkResult _result = VK_NOT_READY;
        bool _ready = false;

        RenderPass(LogicalDevice* device);
    
    public:
        RenderPass();

        void init(
            std::vector<VkFormat> attachment_format,
            VkFormat stencil_format,
            bool stencil_attachment
        );

        VkRenderPass getRenderPass();
        LogicalDevice getDevice();
        VkResult getResult();
        bool isReady();

        void destroy();

    friend LogicalDevice;
    friend SwapChain;
    };
};