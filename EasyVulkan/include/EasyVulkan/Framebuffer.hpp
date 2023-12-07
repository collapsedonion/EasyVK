#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/ImageView.hpp>
#include <vector>

namespace EasyVK{
    
    class Framebuffer{
    private:
        LogicalDevice* _device = nullptr;
        VkFramebuffer _buffer = VK_NULL_HANDLE;
        RenderPass _renderpass;
        VkExtent2D _extent;
        uint32_t _layers;

        VkResult _result = VK_NOT_READY;
        bool _ready = false;
    
        Framebuffer(LogicalDevice* device);

    public:
        Framebuffer();

        void init(std::vector<EasyVK::ImageView> views, VkExtent2D extent, RenderPass renderPass, uint32_t layers);
        void destroy();

        bool isReady();
        VkResult getResult();
        VkExtent2D getExtent();
        VkFramebuffer getFramebuffer();

    friend LogicalDevice;
    };
}