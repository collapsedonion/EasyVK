#pragma once
#include <vulkan/vulkan.h>
#include <EasyVulkan/QueueFamilies.hpp>
#include <EasyVulkan/Instance.hpp>
#include <string>
#include <vector>

namespace EasyVK{

    class LogicalDevice;

    class PhysicalDevice{
    private:
        Instance _instance;
        VkPhysicalDevice _device = VK_NULL_HANDLE;
        VkSurfaceKHR _surface = VK_NULL_HANDLE;
        std::vector<std::string> _extensions;
        bool found = false;

        static bool supportExtensions(
            const VkPhysicalDevice& device, 
            const std::vector<std::string>& extensions
            );

        static bool containsQueue(VkPhysicalDevice device, QueueThat that);
        static bool containsPresentQueue(VkPhysicalDevice device, VkSurfaceKHR surface);

        PhysicalDevice(Instance instance);

        PhysicalDevice(
            Instance instance,
            const std::vector<QueueThat> &supported_families,
            const std::vector<std::string>& suppoted_extensions,
            bool can_present,
            VkSurfaceKHR surface = VK_NULL_HANDLE
        );        
    public:

        PhysicalDevice();

        void find(
            const std::vector<QueueThat> &supported_families,
            const std::vector<std::string>& suppoted_extensions,
            bool can_present,
            VkSurfaceKHR surface = VK_NULL_HANDLE
        );

        bool containsQueue(QueueThat that);
        bool containsPresentQueue(VkSurfaceKHR surface);
        bool containsPresentQueue();
        bool isThisQueueFamily(unsigned int family, QueueThat that);
        bool isThisPresentQueueFamily(unsigned int family);
        bool isFound();

        unsigned int findQueue(QueueThat that, std::vector<unsigned int> skip = {});
        unsigned int findPresentQueue(std::vector<unsigned int> skip = {});

        std::vector<std::string> supportedExtensions();

        VkSurfaceKHR getSurface();
        Instance getInstance();
        VkPhysicalDevice getDevice();
        LogicalDevice* createLogicalDevice(const std::vector<QueueProps>& queues);
        
    public:
        friend Instance;
    };

}