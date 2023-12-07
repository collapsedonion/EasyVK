#pragma once
#include <EasyVulkan/Base.hpp>
#include <vulkan/vulkan.h>
#include <EasyVulkan/ImageView.hpp>
#include <map>

N_EVK{
    class Image{

    private:
        BASIC_MEMBERS;

        LogicalDevice* _device;
        VkImage _image;
        VkDeviceMemory _memory;
        VkExtent3D _extent;
        ImageView _imageView;
        std::map<unsigned int, VkSampler> _samplers;
        
        uint64_t _size;

        Image(LogicalDevice* device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageType type, bool cpuAccess);

    public:
        Image();

        BASIC_METHODS;

        VkExtent3D getExtent();
        VkImage getImage();
        VkDeviceMemory getMemory();
        ImageView getImageView();
        uint64_t getSize();
        VkResult createSampler(unsigned int id, VkFilter filter);
        VkSampler getSampler(unsigned int id);
        void destroySampler(unsigned int id);
        void* map();
        void unMap();
    friend LogicalDevice;
    }; 
}