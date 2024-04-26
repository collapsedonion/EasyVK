//
// Created by Роман  Тимофеев on 26.04.2024.
//
#include <EasyVulkan/SwapChain.hpp>
#include <queue>
#include <set>

std::set<vk::Format> preferableFormats = {
        vk::Format::eR8G8B8A8Srgb,
        vk::Format::eR32G32B32Sfloat,
};

std::set<vk::ColorSpaceKHR> preferableColorSpace = {
        vk::ColorSpaceKHR::eSrgbNonlinear,
        vk::ColorSpaceKHR::eHdr10HlgEXT
};

void EasyVK::SwapChain::setup(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface,
                              std::set<uint32_t> queueFamilies) {

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
    createInfoKhr.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    createInfoKhr.imageSharingMode = queueFamiliesV.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;
    createInfoKhr.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    createInfoKhr.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfoKhr.presentMode = vk::PresentModeKHR::eFifo;
    createInfoKhr.clipped = vk::False;
    createInfoKhr.oldSwapchain = swapChain;

    this->swapChain = device.createSwapchainKHR(createInfoKhr);
    this->queueFamilies = std::move(queueFamilies);
    this->physicalDevice = physicalDevice;
    this->surface = surface;
    this->device = device;
    this->size = vk::Extent3D(createInfoKhr.imageExtent.width, createInfoKhr.imageExtent.height, 1);
    this->format = createInfoKhr.imageFormat;
}

std::vector<EasyVK::Image> EasyVK::SwapChain::getImages() {
    std::vector<vk::Image> images = device.getSwapchainImagesKHR(swapChain);
    std::vector<EasyVK::Image> result;

    for(auto image : images){
        EasyVK::Image newImage = Image();
        newImage.createFromSwapChain(device, format, size, image);
        result.push_back(newImage);
    }

    return result;
}

void EasyVK::SwapChain::recreate() {
    setup(physicalDevice, device, surface, queueFamilies);
}

EasyVK::SwapChain::~SwapChain() {
    if(isKilled()){
        device.destroy(swapChain);
    }
}

uint32_t EasyVK::SwapChain::nextImage() {
    return device.acquireNextImageKHR(swapChain, UINT64_MAX).value;
}

void EasyVK::SwapChain::present(uint32_t imageId, const std::vector<CommandBuffer>& awaitFor) {
    vk::PresentInfoKHR presentInfoKhr = {};
    presentInfoKhr.waitSemaphoreCount = 0;
    presentInfoKhr.swapchainCount = 1;
    presentInfoKhr.pSwapchains = &swapChain;
    presentInfoKhr.pImageIndices = &imageId;
    presentInfoKhr.waitSemaphoreCount = awaitFor.size();
    std::vector<vk::Semaphore> awaitSemaphores;
    awaitSemaphores.reserve(awaitFor.size());
    for(auto& commandBuffer : awaitFor){
        awaitSemaphores.push_back(commandBuffer.mySemaphore);
    }

    presentInfoKhr.pWaitSemaphores = awaitSemaphores.data();

    auto result =  presentQueue.presentKHR(presentInfoKhr);
}

vk::Extent2D EasyVK::SwapChain::getRenderSize() {
    return vk::Extent2D(size.width, size.height);
}
