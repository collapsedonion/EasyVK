#include <EasyVulkan/ImageView.hpp>
#include <EasyVulkan/LogicalDevice.hpp>
#include "ImageView.hpp"

EasyVK::ImageView::ImageView(LogicalDevice* device)
{
    _device = device;
}

void EasyVK::ImageView::init(VkImage image, VkFormat format, VkExtent2D extent, VkImageViewType viewType, uint32_t layer, bool stencil)
{
    _format = format;
    _extent = extent;
    _viewType = viewType;

    VkImageViewCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageCreateInfo.image = image;
    imageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageCreateInfo.subresourceRange.aspectMask =  stencil ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageCreateInfo.subresourceRange.baseMipLevel = 0;
    imageCreateInfo.subresourceRange.levelCount = 1;
    imageCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageCreateInfo.subresourceRange.layerCount = layer;

    _result = vkCreateImageView(_device->getDevice(), &imageCreateInfo, nullptr, &_imageView);

    _ready = _result == VK_SUCCESS;
}

EasyVK::ImageView::ImageView()
{
}

VkImageView EasyVK::ImageView::getImageView()
{
    return _imageView;
}

VkResult EasyVK::ImageView::getResult()
{
    return _result;
}

EasyVK::LogicalDevice EasyVK::ImageView::getDevice()
{
    return *_device;
}

bool EasyVK::ImageView::isReady()
{
    return _ready;
}

VkFormat EasyVK::ImageView::getFormat()
{
    return _format;
}

void EasyVK::ImageView::destroy()
{
    if(_ready){
        vkDestroyImageView(_device->getDevice(), _imageView, nullptr);
        _result = VK_NOT_READY;
        _ready = false;
    }
}
