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

    createInfo.usage |= vk::ImageUsageFlagBits::eSampled;

    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.queueFamilyIndexCount = queueFamilies.size();
    createInfo.pQueueFamilyIndices = queueFamilies.data();

    vk::Image newImage = device.createImage(createInfo);
    vk::MemoryRequirements imageMemRequirements = device.getImageMemoryRequirements(newImage);
    vk::DeviceMemory memory = EasyVK::allocateDeviceMemory(device, physicalDevice, imageMemRequirements, hostAccess);
    device.bindImageMemory(newImage, memory, 0);

    this->layout = createInfo.initialLayout;
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
    if(!createdFromSwapChain) {
        device.freeMemory(this->allocatedMemory);
        device.destroy(this->image);
    }
}

EasyVK::Image::View* EasyVK::Image::getView() {
    auto newView = new View();
    newView->setup(this->device, this->colorFormat, this->size, this->image);
    return newView;
}

void EasyVK::Image::createFromSwapChain(vk::Device device, vk::Format format, vk::Extent3D size, vk::Image image) {
    this->device = device;
    this->colorFormat = format;
    this->size = size;
    this->image = image;
    this->layout = vk::ImageLayout::eUndefined;
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

    vk::SamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.magFilter = vk::Filter::eNearest;
    samplerCreateInfo.minFilter = vk::Filter::eNearest;
    samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerCreateInfo.anisotropyEnable = vk::False;
    samplerCreateInfo.compareEnable = vk::False;
    samplerCreateInfo.borderColor = vk::BorderColor::eFloatTransparentBlack;
    samplerCreateInfo.unnormalizedCoordinates = vk::False;

    this->sampler = device.createSampler(samplerCreateInfo);
    this->device = device;
    this->format = colorFormat;
}

bool EasyVK::Image::View::checkResourceTypeCompatability(ResourceType type)
{
    return (type & DeviceResource::IMAGE_VIEW) || (type & DeviceResource::AUTO); 
}

void EasyVK::Image::View::bindToDescriptorSet(vk::Device device, vk::DescriptorSet set, ResourceType type, uint32_t binding)
{

    vk::DescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageInfo.sampler = this->sampler;
    imageInfo.imageView = this->view;

    vk::WriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = convertResourceTypeToDescriptorType(type);
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.dstSet = set;
    writeDescriptorSet.pImageInfo = &imageInfo;

    device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

EasyVK::DeviceResource::ResourceType EasyVK::Image::View::getVerifiedResourceType(ResourceType type)
{
    if(type & DeviceResource::AUTO){
        return DeviceResource::IMAGE_VIEW;
    }

    return type;
}

EasyVK::Image::View::~View()
{
    this->device.destroy(this->view);
    this->device.destroy(this->sampler);
}

vk::Format EasyVK::Image::getImageFormat(){
    return this->colorFormat;
}
