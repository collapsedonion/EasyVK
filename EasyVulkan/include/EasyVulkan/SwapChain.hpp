#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/LogicalDevice.hpp>
#include <EasyVulkan/ImageView.hpp>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/Semaphore.hpp>
#include <EasyVulkan/Fence.hpp>
#include <EasyVulkan/Image.hpp>

namespace EasyVK{
    class SwapChain{
    private:
        VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
        LogicalDevice* _device;
        VkExtent2D _extent;
        VkPresentModeKHR _presentMode;
        VkSurfaceFormatKHR _format;
        RenderPass _renderPass;
        Image _depthBuffer;
        uint32_t _image_count;

        std::vector<VkImage> _images;
        std::vector<EasyVK::ImageView> _image_views;
        std::vector<Framebuffer> _framebuffers;

        VkResult _result = VK_NOT_READY;
        bool _ready = false;

        static VkSurfaceFormatKHR chooseFormat(std::vector<VkSurfaceFormatKHR> formats, VkSurfaceFormatKHR prefered);
        static VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> formats, VkPresentModeKHR prefered);

        SwapChain(LogicalDevice* device);

    public:

        SwapChain();

        void init(
            unsigned int frameWidth,
            unsigned int frameHeight,
            VkSurfaceFormatKHR prefered_format = {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            VkPresentModeKHR prefered_presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            uint32_t imageArrayLayers = 1,
            VkImageUsageFlags flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VkBool32 clipped = VK_TRUE,
            bool depth_test = false
        );

        VkResult getResult();
        bool isReady();
        VkExtent2D getExtent();
        VkPresentModeKHR getPresentMode();
        VkSurfaceFormatKHR getFormat();
        VkSwapchainKHR getSwapchain();
        LogicalDevice getDevice();
        std::vector<ImageView>& getImageViews();
        RenderPass& getRenderPass();
        std::vector<Framebuffer>& getFramebuffers();
        ImageView getDepthBuffer();
        std::pair<VkResult, uint32_t> nextImageId(uint64_t timeout, Semaphore semaphore = Semaphore(), Fence fence = Fence());

        VkViewport getViewport(float x = 0, float y = 0);

        void destroy();
    
    friend LogicalDevice;
    };
};