#include <set>
#include "PhysicalDevice.hpp"
#include "LogicalDevice.hpp"

bool EasyVK::PhysicalDevice::supportExtensions(
    const VkPhysicalDevice &device, 
    const std::vector<std::string> &extensions
    )
{
    std::set<std::string> neaded_extensions(extensions.begin(), extensions.end());

    unsigned int supportedExtensionsCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedExtensionsCount, nullptr);
    std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionsCount);

    vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedExtensionsCount, supportedExtensions.data());

    for(auto elem : supportedExtensions){
        if(neaded_extensions.contains(elem.extensionName)){
            neaded_extensions.erase(elem.extensionName);
        }
    }

    return neaded_extensions.empty();
}

bool EasyVK::PhysicalDevice::containsQueue(VkPhysicalDevice device, QueueThat that)
{
    unsigned int familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());

    for(auto family : families){
        if(isThisQueue(family, that, 0)){
            return true;
        }
    }

    return false;
}

bool EasyVK::PhysicalDevice::containsPresentQueue(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    unsigned int familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

    for(unsigned int i = 0; i < familyCount; i++){
        if(canThisQueuePresent(surface, device, i)){
            return true;
        }
    }

    return false;
}

EasyVK::PhysicalDevice::PhysicalDevice(Instance instance)
{
    _instance = instance;
}

EasyVK::PhysicalDevice::PhysicalDevice(Instance instance, const std::vector<QueueThat> &supported_families, const std::vector<std::string> &suppoted_extensions, bool can_present, VkSurfaceKHR surface)
{
    _instance = instance;
    _surface = surface;
    find(supported_families, suppoted_extensions, can_present, surface);    
}

EasyVK::PhysicalDevice::PhysicalDevice()
{
}

void EasyVK::PhysicalDevice::find(
    const std::vector<QueueThat> &supported_families,
    const std::vector<std::string> &suppoted_extensions,
    bool can_present,
    VkSurfaceKHR surface)
{
    if(!_instance.valid()) {
        return;
    }

    unsigned int physicalDeviceCount;
    vkEnumeratePhysicalDevices(_instance.getHandledInstance(), &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    
    vkEnumeratePhysicalDevices(_instance.getHandledInstance(), &physicalDeviceCount, physicalDevices.data());

    for(auto elem : physicalDevices){
        bool support_extensions = supportExtensions(elem, suppoted_extensions);
        bool contains_all_queues = true;
        bool present_check = can_present ? containsPresentQueue(elem, surface) : true;

        for(auto that : supported_families){
            if(!containsQueue(elem, that)){
                contains_all_queues = false;
                break;
            }
        }
        
        if(support_extensions && contains_all_queues && present_check){            
            found = true;
            _device = elem;
            _extensions = suppoted_extensions;
            break;
        }
    }
}
bool EasyVK::PhysicalDevice::containsQueue(QueueThat that)
{
    if(found){
        return containsQueue(_device, that);
    }

    return false;
}

bool EasyVK::PhysicalDevice::containsPresentQueue(VkSurfaceKHR surface)
{
    if(found){
        return containsPresentQueue(_device, surface);
    }

    return false;
}

bool EasyVK::PhysicalDevice::containsPresentQueue()
{
    if(_surface != VK_NULL_HANDLE){
        return containsPresentQueue(_surface);
    }

    return false;
}

unsigned int EasyVK::PhysicalDevice::findQueue(QueueThat that, std::vector<unsigned int> skip)
{
    ENUMERATE_DEVICE_QUEUES(_device, queuesCount, queues);

    for(unsigned int i = 0; i < queuesCount; i++){
        if(isThisQueue(queues[i], that, 0)){
            bool noU = false;
            for(auto elem : skip){
                if(i == elem){
                    noU = true;
                    break;
                }
            }

            if(noU){
                continue;
            }
            return i;
        }
    }

    return 0;
}

unsigned int EasyVK::PhysicalDevice::findPresentQueue(std::vector<unsigned int> skip)
{
    DEVICE_QUEUES_COUNT(_device, queuesCount);

    for(unsigned int i = 0; i < queuesCount; i++){
        if(canThisQueuePresent(_surface, _device, i)){
            bool inSkip = false;
            
            for(auto elem : skip){
                if(elem == i){
                    inSkip = true;
                    break;
                }
            }

            if(!inSkip){
                return i;
            }
        }
    }

    return 0;
}

VkPhysicalDevice EasyVK::PhysicalDevice::getDevice()
{
    return _device;
}

std::vector<std::string> EasyVK::PhysicalDevice::supportedExtensions()
{
    return _extensions;
}

VkSurfaceKHR EasyVK::PhysicalDevice::getSurface()
{
    return _surface;
}

EasyVK::Instance EasyVK::PhysicalDevice::getInstance()
{
    return _instance;
}

bool EasyVK::PhysicalDevice::isThisQueueFamily(unsigned int family, QueueThat that){
    ENUMERATE_DEVICE_QUEUES(_device, queuesCount, queues);
    if(family > queuesCount){
        return false;
    }

    return isThisQueue(queues[family], that, 0);
}

bool EasyVK::PhysicalDevice::isThisPresentQueueFamily(unsigned int family){
    return canThisQueuePresent(_surface, _device, family);
}

EasyVK::LogicalDevice* EasyVK::PhysicalDevice::createLogicalDevice(const std::vector<QueueProps>& queues){
    return new LogicalDevice(*this, queues);
}


bool EasyVK::PhysicalDevice::isFound()
{
    return found;
}