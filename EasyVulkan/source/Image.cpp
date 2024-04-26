//
// Created by Роман  Тимофеев on 22.04.2024.
//
#include <EasyVulkan/Image.hpp>
#include "Utils.hpp"

void
EasyVK::Image::allocate(vk::PhysicalDevice physicalDevice, vk::Device device, vk::Format colorFormat, vk::Extent3D size,
                         const std::vector<uint32_t> &queueFamilies, bool hostAccess) {

    vk::SharingMode mode = queueFamilies.size() > 1 ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
    vk::ImageType type = vk::ImageType::e1D;

    if(size.height > 1){
        type = vk::ImageType::e2D;
    }

    if(size.depth > 1){
        type = vk::ImageType::e3D;
    }

    vk::ImageCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eImageCreateInfo;
    createInfo.format = colorFormat;
    createInfo.sharingMode = mode;
    createInfo.extent = size;
    createInfo.tiling = hostAccess ? vk::ImageTiling::eLinear : vk::ImageTiling::eOptimal;
    createInfo.imageType = type;
    createInfo.usage = hostAccess ? vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc : vk::ImageUsageFlagBits::eTransferDst;
    createInfo.initialLayout = vk::ImageLayout::eUndefined;

    if(colorFormat == vk::Format::eD16Unorm || colorFormat == vk::Format::eD16UnormS8Uint || colorFormat == vk::Format::eD24UnormS8Uint || colorFormat == vk::Format::eD32Sfloat || colorFormat == vk::Format::eD32SfloatS8Uint){
        createInfo.usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    }else{
        createInfo.usage |= vk::ImageUsageFlagBits::eColorAttachment;
    }

    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.queueFamilyIndexCount = queueFamilies.size();
    createInfo.pQueueFamilyIndices = queueFamilies.data();

    vk::Image newImage = device.createImage(createInfo);
    vk::MemoryRequirements imageMemRequirements = device.getImageMemoryRequirements(newImage);
    vk::DeviceMemory memory = EasyVK::allocateDeviceMemory(device, physicalDevice, imageMemRequirements, hostAccess);
    device.bindImageMemory(newImage, memory, 0);

    this->device = device;
    this->colorFormat = colorFormat;
    this->size = size;
    this->tiling = createInfo.tiling;
    this->allocatedSize = imageMemRequirements.size;
    this->allocatedMemory = memory;
    this->image = newImage;
}

void *EasyVK::Image::bind() {
    if(createdFromSwapChain){
        throw std::runtime_error("Non bindable resource");
    }

    return device.mapMemory(allocatedMemory, 0, allocatedSize);
}

void EasyVK::Image::unbind(){
    device.unmapMemory(allocatedMemory);
}

EasyVK::Image::~Image() {
    if(isKilled() && !createdFromSwapChain) {
        device.freeMemory(this->allocatedMemory);
        device.destroy(this->image);
    }
}

EasyVK::Image::View EasyVK::Image::getView() {
    auto newView = View();
    newView.setup(this->device, this->colorFormat, this->size, this->image);
    return newView;
}

void EasyVK::Image::createFromSwapChain(vk::Device device, vk::Format format, vk::Extent3D size, vk::Image image) {
    this->device = device;
    this->colorFormat = format;
    this->size = size;
    this->image = image;
    createdFromSwapChain = true;
}

void EasyVK::Image::View::setup(vk::Device device, vk::Format colorFormat, vk::Extent3D size, vk::Image image) {
    vk::ImageViewCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eImageViewCreateInfo;
    createInfo.format = colorFormat;
    createInfo.image = image;
    createInfo.components.a = vk::ComponentSwizzle::eA;
    createInfo.components.b = vk::ComponentSwizzle::eB;
    createInfo.components.g = vk::ComponentSwizzle::eG;
    createInfo.components.r = vk::ComponentSwizzle::eR;
    createInfo.viewType = vk::ImageViewType::e1D;

    if(colorFormat == vk::Format::eD16Unorm || colorFormat == vk::Format::eD32Sfloat){
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    }else if(colorFormat == vk::Format::eD16UnormS8Uint || colorFormat == vk::Format::eD24UnormS8Uint || colorFormat == vk::Format::eD32SfloatS8Uint){
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }
    else {
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    createInfo.subresourceRange.layerCount = 1;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.baseMipLevel = 0;

    if(size.height > 1){
        createInfo.viewType = vk::ImageViewType::e2D;
    }

    if(size.depth > 1){
        createInfo.viewType = vk::ImageViewType::e3D;
    }

    view = device.createImageView(createInfo);
    this->device = device;
    this->format = colorFormat;
}

EasyVK::Image::View::~View() {
    if(isKilled()){
        this->device.destroy(this->view);
    }
}

