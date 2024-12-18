//
// Created by Роман  Тимофеев on 26.04.2024.
//
#include <EasyVulkan/SwapChain.hpp>
#include <queue>
#include <set>

std::set<vk::Format> preferableFormats = {
        vk::Format::eB8G8R8A8Srgb,
        vk::Format::eR8G8B8A8Srgb,
};

std::set<vk::ColorSpaceKHR> preferableColorSpace = {
        vk::ColorSpaceKHR::eSrgbNonlinear,
};

void EasyVK::SwapChain::setup(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface,
                              const std::set<uint32_t>& queueFamilies) {

    vk::SurfaceCapabilitiesKHR surfaceCapabilitiesKhr = physicalDevice.getSurfaceCapabilitiesKHR(surface);

    vk::SwapchainCreateInfoKHR createInfoKhr = {};
    createInfoKhr.surface = surface;

    if(surfaceCapabilitiesKhr.maxImageCount == 0){
        createInfoKhr.minImageCount = surfaceCapabilitiesKhr.minImageCount + 2;
    }else{
        createInfoKhr.minImageCount = std::min(surfaceCapabilitiesKhr.minImageCount + 2, surfaceCapabilitiesKhr.maxImageCount);
    }

    std::vector<vk::SurfaceFormatKHR> supportedFormats = physicalDevice.getSurfaceFormatsKHR(surface);

    std::deque<vk::SurfaceFormatKHR> formats;

    for(auto elem : supportedFormats){
        if(preferableFormats.contains(elem.format) && preferableColorSpace.contains(elem.colorSpace)){
            formats.push_front(elem);
        }else{
            formats.push_back(elem);
        }
    }

    createInfoKhr.imageFormat = formats.front().format;
    createInfoKhr.imageColorSpace = formats.front().colorSpace;

    if(surfaceCapabilitiesKhr.currentExtent == vk::Extent2D{0xFFFFFFFF,0xFFFFFFFF}){
        createInfoKhr.imageExtent = surfaceCapabilitiesKhr.minImageExtent;
    }else{
        createInfoKhr.imageExtent = surfaceCapabilitiesKhr.currentExtent;
    }

    std::vector<uint32_t> queueFamiliesV = std::vector<uint32_t>(queueFamilies.begin(), queueFamilies.end());

    createInfoKhr.imageArrayLayers = 1;
    createInfoKhr.queueFamilyIndexCount = queueFamilies.size();
    createInfoKhr.pQueueFamilyIndices = queueFamiliesV.data();
    createInfoKhr.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
    createInfoKhr.imageSharingMode = queueFamiliesV.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;
    createInfoKhr.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    createInfoKhr.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfoKhr.presentMode = vk::PresentModeKHR::eFifo;
    createInfoKhr.clipped = vk::False;
    createInfoKhr.oldSwapchain = VK_NULL_HANDLE;

    this->swapChain = device.createSwapchainKHR(createInfoKhr);
    this->queueFamilies = std::move(queueFamilies);
    this->physicalDevice = physicalDevice;
    this->surface = surface;
    this->device = device;
    this->size = vk::Extent3D(createInfoKhr.imageExtent.width, createInfoKhr.imageExtent.height, 1);
    this->format = createInfoKhr.imageFormat;

    if(presentaionFence != VK_NULL_HANDLE){
        this->device.waitForFences({this->presentaionFence}, false, UINT64_MAX);
        this->device.destroy(this->presentaionFence);
    }

    vk::FenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    this->presentaionFence = device.createFence(fenceCreateInfo);

}

std::vector<EasyVK::Image*> EasyVK::SwapChain::getImages() {
    std::vector<vk::Image> images = device.getSwapchainImagesKHR(swapChain);
    std::vector<EasyVK::Image*> result;

    for(auto image : images){
        EasyVK::Image* newImage = new Image();
        newImage->createFromSwapChain(device, format, size, image);
        result.push_back(newImage);
    }

    return result;
}

void EasyVK::SwapChain::recreate() {
    this->device.destroy(swapChain);
    setup(physicalDevice, device, surface, queueFamilies);
}

EasyVK::SwapChain::~SwapChain() {
        device.destroy(swapChain);
    device.destroy(presentaionFence);
}

std::pair<uint32_t, vk::Result> EasyVK::SwapChain::nextImage() {
    device.resetFences(presentaionFence);
    auto result = device.acquireNextImageKHR(swapChain, UINT64_MAX, {}, {presentaionFence});

    return {result.value, result.result};
}

void EasyVK::SwapChain::waitForPresentEnd()
{
    device.waitForFences({presentaionFence}, true, UINT64_MAX);
}

vk::Result EasyVK::SwapChain::present(uint32_t imageId)
{
    vk::PresentInfoKHR presentInfoKhr = {};
    presentInfoKhr.waitSemaphoreCount = 0;
    presentInfoKhr.swapchainCount = 1;
    presentInfoKhr.pSwapchains = &swapChain;
    presentInfoKhr.pImageIndices = &imageId;
    
    auto result =  presentQueue.presentKHR(presentInfoKhr);
    return result;
}

vk::Extent2D EasyVK::SwapChain::getRenderSize()
{
    return vk::Extent2D(size.width, size.height);
}
