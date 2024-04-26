//
// Created by Роман  Тимофеев on 20.04.2024.
//

#include <EasyVulkan/PhysicalDevice.hpp>
#include <stdexcept>

void EasyVK::PhysicalDevice::setupWithDesired(EasyVK::PhysicalDevice::DeviceType type,
                                              EasyVK::Features features,
                                              vk::Instance instance) {
    if(set_up){
        return;
    }

    std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    std::vector<vk::PhysicalDevice> remaining;

    for(const auto& device : physicalDevices){
        vk::PhysicalDeviceProperties properties = device.getProperties();

        switch (type){
            case INTEGRATED:
                if(properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu){
                    remaining.push_back(device);
                }
                break;
            case DISCRETE:
                if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
                    remaining.push_back(device);
                }
                break;
            case ANY:
                remaining.push_back(device);
                break;
            default:
                break;
        }
    }

    if(remaining.empty()){
        throw std::runtime_error(std::string("No compatible physical device"));
    }

    physicalDevices = std::move(remaining);
    remaining = {};

    for(const auto& device : physicalDevices){
        std::vector<vk::QueueFamilyProperties> families = device.getQueueFamilyProperties();
        std::vector<vk::ExtensionProperties> extensions = device.enumerateDeviceExtensionProperties();
        bool computeSupport = false;
        bool graphicsSupport = false;
        bool swapChainSupport = false;

        for(const auto& extension : extensions){
            if(std::string(extension.extensionName) == VK_KHR_SWAPCHAIN_EXTENSION_NAME){
                swapChainSupport = true;
            }
        }

        for(const auto& family : families) {
            if (family.queueFlags & vk::QueueFlagBits::eGraphics) {
                graphicsSupport = true;
            }
            if (family.queueFlags & vk::QueueFlagBits::eCompute) {
                computeSupport = true;
            }
        }

        if((features & GRAPHICS) && !graphicsSupport){
            continue;
        }

        if((features & COMPUTE) && !computeSupport){
            continue;
        }

        if((features & SWAP_CHAIN) && !swapChainSupport){
            continue;
        }

        remaining.push_back(device);
    }

    if(remaining.empty()){
        throw std::runtime_error("No compatible physical device");
    }

    this->enabledFeatures = features;
    this->deviceHandler = remaining[0];
    this->instance = instance;
    this->set_up = true;
}

EasyVK::Device EasyVK::PhysicalDevice::createDevice(vk::SurfaceKHR surface) {
    auto newDevice = EasyVK::Device();
    newDevice.setUpWithDesired(this->instance, this->enabledFeatures, this->deviceHandler, surface);
    return newDevice;
}
