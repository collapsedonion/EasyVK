//
// Created by Роман  Тимофеев on 20.04.2024.
//
#include <EasyVulkan/Instance.hpp>

EasyVK::Instance::Instance(const std::string& application_name,
                           EasyVK::Instance::VULKAN_VERSION version,
                           const std::set<std::string>& supported_layers,
                           const std::set<std::string>& supported_extensions) {

    vk::ApplicationInfo applicationInfo = {};
    applicationInfo.sType = vk::StructureType::eApplicationInfo;
    applicationInfo.pApplicationName = application_name.c_str();
    applicationInfo.pEngineName = "Easy_Vulkan";

    switch (version) {
        case V1_0:
            applicationInfo.apiVersion = VK_API_VERSION_1_0;
            break;
    }


    vk::InstanceCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eInstanceCreateInfo;
    createInfo.pApplicationInfo = &applicationInfo;

    std::vector<const char*> layers;
    layers.reserve(supported_layers.size());
    std::vector<const char*> extensions;
    extensions.reserve(supported_extensions.size());

    for(const auto& layer : supported_layers){
        layers.emplace_back(layer.c_str());
    }

    for(const auto& extension : supported_extensions){
        extensions.emplace_back(extension.c_str());
    }

#ifdef __APPLE__
    createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    this->supported_layers = supported_layers;
    this->supported_extensions = supported_extensions;
    instance = vk::createInstance(createInfo);
}

bool EasyVK::Instance::layerEnabled(const std::string &name) {
    return supported_layers.contains(name);
}

bool EasyVK::Instance::extensionEnabled(const std::string &name) {
    return supported_extensions.contains(name);
}

EasyVK::PhysicalDevice * EasyVK::Instance::getPhysicalDevice(
        EasyVK::PhysicalDevice::DeviceType type,
        EasyVK::Features features) {
    auto* device = new EasyVK::PhysicalDevice();
    device->setupWithDesired(type, features, this->instance);
    return device;
}
