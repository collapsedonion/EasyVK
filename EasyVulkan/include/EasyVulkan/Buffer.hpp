#pragma once
#include <vulkan/vulkan.h>

namespace EasyVK{

    class LogicalDevice;

    class Buffer{
    
    private:
        LogicalDevice* _device;
        VkBuffer _buffer;
        VkDeviceMemory _mem;
        uint64_t _size;

        VkResult _result = VK_NOT_READY;

        Buffer(
            LogicalDevice* device,
            uint64_t size,
            bool deviceAccesible,
            VkBufferUsageFlags usage
        );
    public:

        Buffer();

        VkBuffer getBuffer();
        VkDeviceMemory getMemory();
        VkResult getResult();
        uint32_t getSize();
        bool isReady();
        void* map();
        void unMap();

        void destroy();
    
    friend LogicalDevice;
    };
}