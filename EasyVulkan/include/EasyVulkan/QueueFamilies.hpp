#pragma once
#include <vulkan/vulkan.h>

namespace EasyVK{

#define ENUMERATE_DEVICE_QUEUES(device, counter, holder) unsigned int counter; vkGetPhysicalDeviceQueueFamilyProperties(device, &counter, nullptr);  std::vector<VkQueueFamilyProperties> holder(counter); vkGetPhysicalDeviceQueueFamilyProperties(device, &counter, holder.data());
#define DEVICE_QUEUES_COUNT(device, counter) unsigned int counter; vkGetPhysicalDeviceQueueFamilyProperties(device, &counter, nullptr);

enum class QueueThat{
    GRAPHICS = 0x00000001,
    COMPUTE = 0x00000002,
    TRANSFER = 0x00000004,
    SPARSE_BINDING = 0x00000008,
    ZERO = 0x0
};

QueueThat operator | (QueueThat a, QueueThat b);
QueueThat operator & (QueueThat a, QueueThat b);
QueueThat& operator |= (QueueThat& a, QueueThat b);

struct QueueProps{
    QueueThat flags;
    bool present = false;
    bool unique = false;
    unsigned int count = 1;
    float prioriety = 1.0f;
};

bool isThisQueue(
    VkQueueFamilyProperties properties, 
    QueueThat queue_flags, 
    unsigned int canProvideXQueues
    );

bool canThisQueuePresent(
    VkSurfaceKHR surface, 
    VkPhysicalDevice device, 
    unsigned int queue
    );
    
}