#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/DescriptorLayout.hpp>
#include <EasyVulkan/Buffer.hpp>
#include <EasyVulkan/Image.hpp>
#include <vector>

namespace EasyVK
{

    class LogicalDevice;
    
    class DescriptorPool{
    
    private:
        LogicalDevice* _device;
        VkDescriptorPool _pool;
        std::vector<VkDescriptorSet> _sets;

        VkResult _result = VK_NOT_READY;

        DescriptorPool(LogicalDevice* device, uint32_t maxSize, std::vector<VkDescriptorType> types, std::vector<uint32_t> count);

    public:
        DescriptorPool();

        VkResult getResult();
        bool isReady();
        VkDescriptorPool getPool();
        VkResult allocateAtIndex(long long i, VkDescriptorSetLayout layout);
        void configureAtIndex(long long i, uint32_t binding, VkDescriptorType type, Buffer buffer = {}, Image image = {}, uint32_t sampler_id = 0,uint32_t arrayElem = 0);
        void freeAtIndex(long long i);
        VkDescriptorSet getDescriptor(long long i);

        void destroy();

    friend LogicalDevice;
    };
} 
