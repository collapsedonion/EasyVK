#include <EasyVulkan/Framebuffer.hpp>
#include <EasyVulkan/LogicalDevice.hpp>

EasyVK::Framebuffer::Framebuffer(LogicalDevice * device)
{
    _device = device;
}

EasyVK::Framebuffer::Framebuffer()
{
}

void EasyVK::Framebuffer::init(std::vector<EasyVK::ImageView> views, VkExtent2D extent, RenderPass renderPass, uint32_t layers)
{
    if(_device == nullptr){
        return;
    }

    if(!_device->ready()){
        return;
    }

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.height = extent.height;
    createInfo.width = extent.width;
    createInfo.renderPass = renderPass.getRenderPass();
    createInfo.layers = layers;
    createInfo.attachmentCount = views.size();
    std::vector<VkImageView> vk_views;
    for(auto elem : views){
        vk_views.push_back(elem.getImageView());
    }

    createInfo.pAttachments = vk_views.data();

    _result = vkCreateFramebuffer(_device->getDevice(), &createInfo, nullptr, &_buffer);

    _ready = _result == VK_SUCCESS;
    _extent = extent;
    _renderpass = renderPass;
    _layers = layers;
}

void EasyVK::Framebuffer::destroy()
{
    if(_ready){
        _ready = false;
        _result = VK_NOT_READY;
        vkDestroyFramebuffer(_device->getDevice(), _buffer, nullptr);
    }
}

bool EasyVK::Framebuffer::isReady()
{
    return _ready;
}

VkResult EasyVK::Framebuffer::getResult()
{
    return _result;
}

VkExtent2D EasyVK::Framebuffer::getExtent()
{
    return _extent;
}

VkFramebuffer EasyVK::Framebuffer::getFramebuffer()
{
    return _buffer;
}
