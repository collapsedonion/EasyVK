//
// Created by Роман  Тимофеев on 20.04.2024.
//

#include <EasyVulkan/Device.hpp>
#include <unordered_map>
#include <stdexcept>
#include <queue>
#include <set>

void EasyVK::Device::setUpWithDesired(vk::Instance instance, Features features, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {

    bool computeQueueNeeded = false;
    bool graphicsQueueNeeded = false;
    bool transferQueueNeeded = false;
    bool presentQueueNeeded = false;

    if(features & GRAPHICS){
        graphicsQueueNeeded = true;
        transferQueueNeeded = true;
    }

    if(features & COMPUTE){
        computeQueueNeeded = true;
        transferQueueNeeded = true;
    }

    if(features & SWAP_CHAIN){
        presentQueueNeeded = true;

        if(surface == VK_NULL_HANDLE){
            throw std::runtime_error("vk::Surface is required when creating device with swap chain support");
        }
    }

    std::vector<std::pair<uint32_t, vk::QueueFamilyProperties*>> allComputeFamilies;
    std::vector<std::pair<uint32_t, vk::QueueFamilyProperties*>> allGraphicsFamilies;
    std::vector<std::pair<uint32_t, vk::QueueFamilyProperties*>> allTransferFamilies;
    std::vector<std::pair<uint32_t, vk::QueueFamilyProperties*>> allPresentFamilies;
    std::vector<vk::QueueFamilyProperties> allFamilies = physicalDevice.getQueueFamilyProperties();

    if(computeQueueNeeded){
        for(long long i = 0; i < allFamilies.size(); i++){
            if(allFamilies[i].queueFlags & vk::QueueFlagBits::eCompute){
                allComputeFamilies.emplace_back(i, &allFamilies[i]);
            }
        }
    }

    if(graphicsQueueNeeded){
        for(long long i = 0; i < allFamilies.size(); i++){
            if(allFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics){
                allGraphicsFamilies.emplace_back(i, &allFamilies[i]);
            }
        }
    }

    if(transferQueueNeeded){
        for(long long i = 0; i < allFamilies.size(); i++){
            if(allFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer){
                allTransferFamilies.emplace_back(i, &allFamilies[i]);
            }
        }
    }

    if(presentQueueNeeded){
        for(long long i = 0; i < allFamilies.size(); i++){
            if(physicalDevice.getSurfaceSupportKHR(i, surface)){
                allPresentFamilies.emplace_back(i, &allFamilies[i]);
            }
        }
    }

    std::unordered_map<std::string, uint32_t> selectedQueues = {};

    if(graphicsQueueNeeded){
        std::deque<std::pair<uint32_t, vk::QueueFamilyProperties*>> families;

        for(auto family : allGraphicsFamilies){
            if((family.second->queueFlags & vk::QueueFlagBits::eCompute) || (family.second->queueFlags & vk::QueueFlagBits::eTransfer) || (physicalDevice.getSurfaceSupportKHR(family.first, surface))){
                families.push_back(family);
            }else{
                families.push_front(family);
            }
        }

        if(families.empty()){
            delete this;
            throw std::runtime_error("Device doesn't have an graphics queue");
        }

        selectedQueues["Graphics"] = families[0].first;
        families[0].second->queueCount--;
    }

    if(computeQueueNeeded){
        std::deque<std::pair<uint32_t, vk::QueueFamilyProperties*>> families;

        for(auto family : allComputeFamilies){
            if(family.second->queueCount == 0){
                continue;
            }

            if((family.second->queueFlags & vk::QueueFlagBits::eGraphics) || (family.second->queueFlags & vk::QueueFlagBits::eTransfer) || (physicalDevice.getSurfaceSupportKHR(family.first, surface))){
                families.push_back(family);
            }else{
                families.push_front(family);
            }
        }

        if(families.empty()){
            delete this;
            throw std::runtime_error("Device doesn't have an compute queue");
        }

        selectedQueues["Compute"] = families[0].first;
        families[0].second->queueCount--;
    }

    if(transferQueueNeeded){
        std::deque<std::pair<uint32_t, vk::QueueFamilyProperties*>> families;

        for(auto family : allTransferFamilies){
            if(family.second->queueCount == 0){
                continue;
            }

            if((family.second->queueFlags & vk::QueueFlagBits::eCompute) || (family.second->queueFlags & vk::QueueFlagBits::eGraphics) || (physicalDevice.getSurfaceSupportKHR(family.first, surface))){
                families.push_back(family);
            }else{
                families.push_front(family);
            }
        }

        if(families.empty()){
            delete this;
            throw std::runtime_error("Device doesn't have an transfer queue");
        }

        selectedQueues["Transfer"] = families[0].first;
        families[0].second->queueCount--;
    }

    if(presentQueueNeeded){
        std::deque<std::pair<uint32_t, vk::QueueFamilyProperties*>> families;

        for(auto family : allPresentFamilies){
            if(family.second->queueCount == 0){
                continue;
            }

            if((family.second->queueFlags & vk::QueueFlagBits::eCompute) || (family.second->queueFlags & vk::QueueFlagBits::eGraphics) || (family.second->queueFlags & vk::QueueFlagBits::eTransfer)){
                families.push_back(family);
            }else{
                families.push_front(family);
            }
        }

        if(families.empty()){
            delete this;
            throw std::runtime_error("Device doesn't have an transfer queue");
        }

        selectedQueues["Present"] = families[0].first;
        families[0].second->queueCount--;
    }

    std::unordered_map<uint32_t, uint32_t> allocCount;
    std::unordered_map<uint32_t, std::vector<float>> queuePriorities;
    std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> queueMap;

    allFamilies = physicalDevice.getQueueFamilyProperties();

    for(const auto& selectedQueue : selectedQueues){
        if(allocCount.contains(selectedQueue.second)){
            allocCount[selectedQueue.second]++;
            queueMap[selectedQueue.first] = {selectedQueue.second, allocCount[selectedQueue.second] - 1};

        }else{
            allocCount[selectedQueue.second] = 1;
            queueMap[selectedQueue.first] = {selectedQueue.second, 0};
            queuePriorities.insert({selectedQueue.second, {}});
        }
    }

    for(auto& priority : queuePriorities){
        uint32_t maxAllocCount = allFamilies[priority.first].queueCount;

        priority.second.resize(maxAllocCount, 1.0f);
    }

    std::vector<const char*> enabledExtensions;

    if(features & SWAP_CHAIN){
        enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

#ifdef __APPLE__
    enabledExtensions.push_back("VK_KHR_portability_subset");
#endif

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    for(const auto& queueAllocInfo : allocCount){
        vk::DeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
        queueCreateInfo.queueFamilyIndex = queueAllocInfo.first;
        queueCreateInfo.queueCount = allFamilies[queueAllocInfo.first].queueCount;
        queueCreateInfo.pQueuePriorities = queuePriorities[queueAllocInfo.first].data();
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::DeviceCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eDeviceCreateInfo;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    this->device = physicalDevice.createDevice(createInfo);

    std::vector<vk::Queue> fetchedQueues;
    if(graphicsQueueNeeded){
        uint32_t myFamilyId = queueMap["Graphics"].first;
        uint32_t myTaskNumber = queueMap["Graphics"].second;
        uint32_t myFamilyMaxCount = allFamilies[myFamilyId].queueCount;
        uint32_t perTaskQueueCount = myFamilyMaxCount / allocCount[myFamilyId];

        for(uint32_t i = myTaskNumber * perTaskQueueCount; i < (myTaskNumber * perTaskQueueCount) + perTaskQueueCount; i++){
            fetchedQueues.push_back(this->device.getQueue(myFamilyId, i));
        }

        this->graphics = {myFamilyId, fetchedQueues};
    }

    if(computeQueueNeeded){
        fetchedQueues.clear();
        uint32_t myFamilyId = queueMap["Compute"].first;
        uint32_t myTaskNumber = queueMap["Compute"].second;
        uint32_t myFamilyMaxCount = allFamilies[myFamilyId].queueCount;
        uint32_t perTaskQueueCount = myFamilyMaxCount / allocCount[myFamilyId];

        for(uint32_t i = myTaskNumber * perTaskQueueCount; i < (myTaskNumber * perTaskQueueCount) + perTaskQueueCount; i++){
            fetchedQueues.push_back(this->device.getQueue(myFamilyId, i));
        }

        this->compute = {myFamilyId, fetchedQueues};
    }

    if(transferQueueNeeded){
        fetchedQueues.clear();
        uint32_t myFamilyId = queueMap["Transfer"].first;
        uint32_t myTaskNumber = queueMap["Transfer"].second;
        uint32_t myFamilyMaxCount = allFamilies[myFamilyId].queueCount;
        uint32_t perTaskQueueCount = myFamilyMaxCount / allocCount[myFamilyId];

        for(uint32_t i = myTaskNumber * perTaskQueueCount; i < (myTaskNumber * perTaskQueueCount) + perTaskQueueCount; i++){
            fetchedQueues.push_back(this->device.getQueue(myFamilyId, i));
        }

        this->transfer = {myFamilyId, fetchedQueues};
    }

    if(presentQueueNeeded){
        fetchedQueues.clear();
        uint32_t myFamilyId = queueMap["Present"].first;
        uint32_t myTaskNumber = queueMap["Present"].second;
        uint32_t myFamilyMaxCount = allFamilies[myFamilyId].queueCount;
        uint32_t perTaskQueueCount = myFamilyMaxCount / allocCount[myFamilyId];

        for(uint32_t i = myTaskNumber * perTaskQueueCount; i < (myTaskNumber * perTaskQueueCount) + perTaskQueueCount; i++){
            fetchedQueues.push_back(this->device.getQueue(myFamilyId, i));
        }
        this->presentQueue = {myFamilyId, fetchedQueues};
    }

    for(const auto& queueFamily : allocCount){
        vk::CommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
        commandPoolCreateInfo.queueFamilyIndex = queueFamily.first;
        commandPoolCreateInfo.flags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        commandPools.insert({queueFamily.first, device.createCommandPool(commandPoolCreateInfo)});
    }

    this->enabledFeatures = features;
    this->set_up = true;
    this->physicalDevice = physicalDevice;
}

EasyVK::Device::~Device(){
    for(const auto& commandPool : commandPools){
        this->device.destroy(commandPool.second);
    }

    this->device.destroy();
    this->set_up = false;
    
}

EasyVK::CommandBuffer*
EasyVK::Device::createCommandBuffer(uint32_t family, vk::Queue queue, EasyVK::Features requiredFeatures) {
    if(requiredFeatures & this->enabledFeatures){
        auto newBuffer = new EasyVK::CommandBuffer();
        newBuffer->allocateCommandBuffer(this->commandPools[family], queue, this->device);
        return newBuffer;
    }

    return {};
}

EasyVK::CommandBuffer* EasyVK::Device::createGraphicsCommandBuffer(uint32_t queueId) {
    return createCommandBuffer(graphics.first, graphics.second[queueId], FeaturesBits::GRAPHICS);
}

EasyVK::CommandBuffer* EasyVK::Device::createComputeCommandBuffer(uint32_t queueId) {
    return createCommandBuffer(compute.first, compute.second[queueId], FeaturesBits::COMPUTE);
}

EasyVK::CommandBuffer* EasyVK::Device::createTransferCommandBuffer(uint32_t queueId) {
    return createCommandBuffer(transfer.first, transfer.second[queueId], FeaturesBits::COMPUTE | FeaturesBits::GRAPHICS);
}

EasyVK::Buffer* EasyVK::Device::createBuffer(size_t size, EasyVK::Device::BufferAccessFlags accessFlags) {
    auto newBuffer = new Buffer();
    std::set<uint32_t> friendFamilies;
    if(accessFlags & Compute){
        friendFamilies.emplace(compute.first);
    }

    if(accessFlags & Graphics){
        friendFamilies.emplace(graphics.first);
    }

    if(accessFlags & Transfer){
        friendFamilies.emplace(transfer.first);
    }

    newBuffer->allocate(this->physicalDevice, this->device, size, accessFlags & Host, std::vector<uint32_t>(friendFamilies.begin(), friendFamilies.end()));
    return newBuffer;
}

EasyVK::Image* EasyVK::Device::createImage(vk::Extent3D size, vk::Format colorFormat,
                                           EasyVK::Device::BufferAccessFlags accessFlags) {
    auto newImage = new Image();
    std::set<uint32_t> friendFamilies;
    if(accessFlags & Compute){
        friendFamilies.emplace(compute.first);
    }

    if(accessFlags & Graphics){
        friendFamilies.emplace(graphics.first);
    }

    if(accessFlags & Transfer){
        friendFamilies.emplace(transfer.first);
    }
    newImage->allocate(physicalDevice, device, colorFormat, size, std::vector<uint32_t>(friendFamilies.begin(), friendFamilies.end()), accessFlags & Host);
    return newImage;
}

EasyVK::Shader* EasyVK::Device::createShader(const std::vector<uint32_t>& shaderCode) {
    auto newShader = new Shader();
    newShader->setup(this->device, shaderCode);
    return newShader;
}

EasyVK::ResourceDescriptor* EasyVK::Device::createResourceDescriptor(
        const std::map<uint32_t, ResourceDescriptor::ResourceBindingDescriptor> &bindings) {
    auto newDescriptor = new ResourceDescriptor();
    newDescriptor->setup(device, bindings);
    return newDescriptor;
}

EasyVK::ComputePipeline* EasyVK::Device::createComputePipeline(std::pair<Shader*, std::string> shader,
                                                               EasyVK::ResourceDescriptor* descriptor) {
    auto newPipeline = new ComputePipeline();
    newPipeline->setup({shader.first, shader.second}, descriptor, this->device);
    return newPipeline;
}

EasyVK::RenderPass* EasyVK::Device::createRenderPass(const std::vector<std::pair<Image*, RenderPass::ImageOp>>& images) {
    auto renderPass = new EasyVK::RenderPass();
    renderPass->setup(images, device);
    return renderPass;
}

EasyVK::SwapChain* EasyVK::Device::createSwapChain(vk::SurfaceKHR surfaceKhr, uint32_t queueId) {
    if(!(enabledFeatures & EasyVK::FeaturesBits::SWAP_CHAIN)){
        throw std::runtime_error("Can not create swap chain on that device");
    }

    auto swapChain = new EasyVK::SwapChain();
    swapChain->presentQueue = presentQueue.second[queueId];
    swapChain->setup(physicalDevice, device, surfaceKhr, {presentQueue.first, graphics.first});
    return swapChain;
}

uint32_t EasyVK::Device::getQueueCountFor(EasyVK::Device::BufferAccessFlagBits flagBits) {

    switch (flagBits) {
        case Compute:
            return compute.second.size();

        case Graphics:
            return graphics.second.size();

        case Transfer:
            return transfer.second.size();

        case Present:
            return presentQueue.second.size();
    }

    throw std::runtime_error("Invalid flag bits");
}
