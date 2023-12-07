#include "SwapChain.hpp"
#include <EasyVulkan/SwapChain.hpp>
#include <EasyVulkan/RenderPass.hpp>

VkSurfaceFormatKHR EasyVK::SwapChain::chooseFormat(std::vector<VkSurfaceFormatKHR> formats, VkSurfaceFormatKHR prefered)
{
    VkSurfaceFormatKHR selected;

    for(auto elem : formats){
        if(elem.colorSpace == prefered.colorSpace && elem.format == prefered.format){
            return prefered;
        }
    }

    for(auto elem : formats){
        if(elem.format == VK_FORMAT_R8G8B8_UINT)
        {
            return elem;
        }
    }

    return formats[0];
}

VkPresentModeKHR EasyVK::SwapChain::choosePresentMode(std::vector<VkPresentModeKHR> formats, VkPresentModeKHR prefered)
{
    for(const auto& mode : formats){
        if(mode == prefered){
            return mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

EasyVK::SwapChain::SwapChain(LogicalDevice* device)
{
    _device = device;
}

EasyVK::SwapChain::SwapChain()
{
}

void EasyVK::SwapChain::init(
    unsigned int frameWidth,
    unsigned int frameHeight,
    VkSurfaceFormatKHR prefered_format,
    VkPresentModeKHR prefered_presentMode,
    uint32_t imageArrayLayers,
    VkImageUsageFlags flags,
    VkBool32 clipped,
    bool depth_test)
{
    if(!_device->ready()){
        return;
    }

    destroy();

    _extent.width = frameWidth;
    _extent.height = frameHeight;

    VkSurfaceCapabilitiesKHR capabilities = _device->getCapabilities();
    auto presentModes = _device->getSupportedPresentModes();
    auto formats = _device->getSupportedFormats();

    _presentMode = choosePresentMode(presentModes, prefered_presentMode);
    _format = chooseFormat(formats, prefered_format);

    uint32_t image_count = capabilities.minImageCount + 1;
    if(capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount){
        image_count = capabilities.maxImageCount;
    }

    _image_count = image_count;

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = _device->getPhysicalDevice().getSurface();
    swapchainCreateInfo.minImageCount = image_count;
    swapchainCreateInfo.imageFormat = _format.format;
    swapchainCreateInfo.imageColorSpace = _format.colorSpace;
    swapchainCreateInfo.imageExtent = _extent;
    swapchainCreateInfo.imageArrayLayers = imageArrayLayers;
    swapchainCreateInfo.flags = flags;

    uint32_t queueFamilies[2] = {_device->getQueue(QueueThat::GRAPHICS).getFamily(), _device->getPresentQueue().getFamily()};

    if(queueFamilies[0] != queueFamilies[1]){
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilies;
    }else{
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainCreateInfo.preTransform = capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = _presentMode;
    swapchainCreateInfo.clipped = clipped;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if(depth_test){
        _depthBuffer = _device->createImage({_extent.width, _extent.height, 1}, VK_FORMAT_D32_SFLOAT, false, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
    _result = vkCreateSwapchainKHR(_device->getDevice(), &swapchainCreateInfo, nullptr, &_swapchain);

    _ready = _result == VK_SUCCESS;

    _renderPass = RenderPass(_device);

    if(_ready){
        uint32_t r_ic;
        vkGetSwapchainImagesKHR(_device->getDevice(), _swapchain, &r_ic, nullptr);
        _images.resize(r_ic);
        vkGetSwapchainImagesKHR(_device->getDevice(), _swapchain, &r_ic, _images.data());

        _renderPass.init({_format.format}, depth_test ? VK_FORMAT_D32_SFLOAT: VK_FORMAT_UNDEFINED, depth_test);

        for(auto elem : _images){
            EasyVK::ImageView iv(_device);
            iv.init(elem, _format.format, _extent, VK_IMAGE_VIEW_TYPE_2D, imageArrayLayers);
            _image_views.push_back(iv);
        }

        for(auto elem : _image_views){
            std::vector<ImageView> views = {elem};
            if(depth_test){
                views.push_back(_depthBuffer.getImageView());
            }
            Framebuffer fb = _device->createFrameBuffer(views, _extent, _renderPass, imageArrayLayers);
            _framebuffers.push_back(fb);
        }
    }
}

VkResult EasyVK::SwapChain::getResult()
{
    return _result;
}

bool EasyVK::SwapChain::isReady()
{
    return _ready;
}

VkExtent2D EasyVK::SwapChain::getExtent()
{
    return _extent;
}

VkPresentModeKHR EasyVK::SwapChain::getPresentMode()
{
    return _presentMode;
}

VkSurfaceFormatKHR EasyVK::SwapChain::getFormat()
{
    return _format;
}

VkSwapchainKHR EasyVK::SwapChain::getSwapchain()
{
    return _swapchain;
}

EasyVK::LogicalDevice EasyVK::SwapChain::getDevice()
{
    return *_device;
}

std::vector<EasyVK::ImageView>& EasyVK::SwapChain::getImageViews()
{
    return _image_views;
}

void EasyVK::SwapChain::destroy()
{
    if(_ready){

        for(auto elem : _framebuffers){
            elem.destroy();
        }

        for(auto elem : _image_views){
            elem.destroy();
        }
        _renderPass.destroy();  
        _depthBuffer.destroy();      
        vkDestroySwapchainKHR(_device->getDevice(), _swapchain, nullptr);
        _ready = false;
        _result = VK_NOT_READY;
    }
}

std::pair<VkResult, uint32_t> EasyVK::SwapChain::nextImageId(uint64_t timeout, Semaphore semaphore, Fence fence)
{
    uint32_t id;
    VkSemaphore sem = semaphore.isReady() ? semaphore.getSemaphore() : VK_NULL_HANDLE;
    VkFence fen = fence.isReady() ? fence.getFence() : VK_NULL_HANDLE;
    VkResult res = vkAcquireNextImageKHR(_device->getDevice(), _swapchain, timeout, sem, fen, &id);

    return {res, id};
}

EasyVK::ImageView EasyVK::SwapChain::getDepthBuffer()
{
    return _depthBuffer.getImageView();
}

VkViewport EasyVK::SwapChain::getViewport(float x, float y)
{
    VkViewport viewport{};
    viewport.x = x;
    viewport.y = y;
    viewport.width = _extent.width;
    viewport.height = _extent.height;
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;
    return viewport;
}

EasyVK::RenderPass& EasyVK::SwapChain::getRenderPass()
{
    return _renderPass;
}

std::vector<EasyVK::Framebuffer> &EasyVK::SwapChain::getFramebuffers()
{
    return _framebuffers;
}
