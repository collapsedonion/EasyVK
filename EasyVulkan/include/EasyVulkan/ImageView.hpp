#pragma once
#include <vulkan/vulkan.h>

namespace EasyVK{
    class SwapChain;
    class Image;
    class LogicalDevice;

    class ImageView{    
    private:
        VkImageView _imageView = VK_NULL_HANDLE;
        VkResult _result = VK_NOT_READY;
        bool _ready = false;
        LogicalDevice* _device = nullptr;
        VkFormat _format;
        VkImageViewType _viewType;
        VkExtent2D _extent;
        uint32_t _layers;

        ImageView(LogicalDevice* device);

        void init(VkImage image, VkFormat format, VkExtent2D extent, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t layers = 1, bool stencil = false);
    public:
        ImageView();

        VkImageView getImageView();
        VkResult getResult();
        LogicalDevice getDevice();
        bool isReady();
        VkFormat getFormat();

        void destroy();

    friend SwapChain;
    friend Image;
    };
}