#include <EasyVulkan/Image.hpp>
#include <EasyVulkan/LogicalDevice.hpp>

BASIC_METHOD__I(Image)

EasyVK::Image::Image(){
}

EasyVK::Image::Image(LogicalDevice *device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageType type, bool cpuAccess)
{
    _device = device;
    _extent = extent;
    VkImageCreateInfo imageCI{};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.extent = extent;
    imageCI.arrayLayers = 1;
    imageCI.mipLevels = 1;
    imageCI.imageType = type;
    imageCI.usage = usage;
    imageCI.format = format;
    imageCI.tiling = cpuAccess ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
    imageCI.initialLayout = format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT ? VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
    imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCI.samples = VK_SAMPLE_COUNT_1_BIT;

    _result = vkCreateImage(_device->getDevice(), &imageCI, nullptr, &_image);

    if(!isReady()){
        return;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device->getDevice(), _image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = _device->findMemoryType(memRequirements.memoryTypeBits, cpuAccess ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _result = vkAllocateMemory(_device->getDevice(), &allocInfo, nullptr, &_memory);

    if(!isReady()){
        vkDestroyImage(_device->getDevice(), _image, nullptr);
        return;
    }

    vkBindImageMemory(_device->getDevice(), _image, _memory, 0);
    _size = memRequirements.size;

    _imageView = ImageView(_device);
    _imageView.init(_image, format, {extent.width, extent.height}, (VkImageViewType)type, 1, format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT);
}

VkImage EasyVK::Image::getImage()
{
    return _image;
}

VkDeviceMemory EasyVK::Image::getMemory(){
    return _memory;
}

void* EasyVK::Image::map(){
    void* toRet;
    vkMapMemory(_device->getDevice(), _memory, 0, _size, 0, &toRet);
    return toRet;
}

void EasyVK::Image::unMap(){
    vkUnmapMemory(_device->getDevice(), _memory);
}

uint64_t EasyVK::Image::getSize(){
    return _size;
}

void EasyVK::Image::destroy(){
    if(isReady()){
        _result = VK_NOT_READY;
        _imageView.destroy();
        vkDestroyImage(_device->getDevice(), _image, nullptr);
        vkFreeMemory(_device->getDevice(), _memory, nullptr);
    }
}

VkExtent3D EasyVK::Image::getExtent(){
    return _extent;
}

EasyVK::ImageView EasyVK::Image::getImageView()
{
    return _imageView;
}

void EasyVK::Image::destroySampler(unsigned int id)
{
    if(_samplers.count(id)){
        vkDestroySampler(_device->getDevice(), _samplers[id],nullptr);
        _samplers.erase(id);
    }
}

VkResult EasyVK::Image::createSampler(unsigned int id, VkFilter filter)
{
    if(_samplers.count(id)){
        destroySampler(id);
    }

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = filter;
    createInfo.minFilter = filter;

    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    createInfo.anisotropyEnable = VK_FALSE;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;

    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.mipLodBias = 0.0f;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;

    VkSampler sampler;
    VkResult res = vkCreateSampler(_device->getDevice(), &createInfo, nullptr, &sampler);
    if(res == VK_SUCCESS){
        _samplers[id] = sampler;
    }

    return res;
}

VkSampler EasyVK::Image::getSampler(unsigned int id)
{
    return _samplers[id];
}
