#include "EasyVulkan/QueueFamilies.hpp"
#include <vector>
#include "QueueFamilies.hpp"

namespace EasyVK{

    QueueThat operator|(QueueThat a, QueueThat b) {
        return (QueueThat)((unsigned )a | (unsigned int)b);
    }

    QueueThat operator&(QueueThat a, QueueThat b)
    {
        return (QueueThat)((unsigned )a & (unsigned int)b);
    }

    QueueThat &operator|=(QueueThat &a, QueueThat b)
    {
        a = a | b;
        return a;
    }

    bool isThisQueue(
        VkQueueFamilyProperties properties,
        QueueThat queue_flags,
        unsigned int canProvideXQueues)
    {

        bool flag_provided = properties.queueFlags & (int)queue_flags;
        bool canProvideQueues = properties.queueCount >= canProvideXQueues;

        if (flag_provided && canProvideQueues)
        {
            return true;
        }

        return false;
    }

bool canThisQueuePresent(
    VkSurfaceKHR surface, 
    VkPhysicalDevice device, 
    unsigned int queue
    )
{
    VkBool32 support;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, queue, surface, &support);

    return (bool)support;
}

}