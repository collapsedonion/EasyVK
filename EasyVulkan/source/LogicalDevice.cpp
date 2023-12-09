#include "LogicalDevice.hpp"
#include <map>
#include <EasyVulkan/SwapChain.hpp>
#include <stdexcept>

EasyVK::LogicalDevice::LogicalDevice(const PhysicalDevice &device)
{
    _my_device = device;
}

EasyVK::LogicalDevice::LogicalDevice(const PhysicalDevice &device, const std::vector<QueueProps> &queues)
{
    _my_device = device;
    init(queues);
}

EasyVK::LogicalDevice::LogicalDevice()
{
}

void EasyVK::LogicalDevice::init(const std::vector<QueueProps> &queues)
{
    destroy();
    std::map<unsigned int, QueueSetting> queuesToCreate;

    for(auto queue : queues){
        unsigned int index;
        if(queue.present){
            std::vector<unsigned int> skip;
            while(true){
                index = _my_device.findPresentQueue(skip);
                if(queuesToCreate.count(index)){
                    ENUMERATE_DEVICE_QUEUES(_my_device.getDevice(), qC,qP);
                    if(qP[index].queueCount < queuesToCreate[index].count + 1){
                        skip.push_back(index);
                        continue;
                    }
                }
                break;
            }
        }else{
            std::vector<unsigned int> skip = {};

            if(queue.unique){
                for(auto elem : queuesToCreate){
                    skip.push_back(elem.first);
                }
            }

            while(true){
                index = _my_device.findQueue(queue.flags, skip);
                if(queuesToCreate.count(index)){
                    ENUMERATE_DEVICE_QUEUES(_my_device.getDevice(),qC,qP);
                    if(qP[index].queueCount < queuesToCreate[index].count + 1){
                        skip.push_back(index);
                        continue;
                    }
                }
                break;
            }
        }

        if(!queuesToCreate.contains(index)){
            queuesToCreate.insert({index, {}});
        }

        queuesToCreate[index].count += queue.count;
        queuesToCreate[index].priorieties.push_back(queue.prioriety);
        queuesToCreate[index].flags |= queue.flags;
        queuesToCreate[index].canPresent =   queuesToCreate[index].canPresent ?  queuesToCreate[index].canPresent : queue.present;
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for(auto& elem : queuesToCreate){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = elem.first;
        queueCreateInfo.queueCount = elem.second.count;
        queueCreateInfo.pQueuePriorities = queuesToCreate[elem.first].priorieties.data();
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    VkPhysicalDeviceFeatures deviceFeatures{};

    std::vector<std::string> extensions = _my_device.supportedExtensions();

    std::vector<const char*> c_extensions;
    for(auto& elem : extensions){
        c_extensions.push_back(elem.data());
    }

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledExtensionCount = c_extensions.size();
    createInfo.ppEnabledExtensionNames = c_extensions.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    _result = vkCreateDevice(_my_device.getDevice(), &createInfo, nullptr, &_device);

    if(_result == VK_SUCCESS){
        for(auto elem : queuesToCreate){
            Queue newQueue;
            newQueue._log_device = this;
            newQueue.family = elem.first;
            newQueue._flags = elem.second.flags;
            newQueue.canPresent = elem.second.canPresent;
            vkGetDeviceQueue(_device, elem.first, 0, &newQueue._queue);
            _savedQueues.push_back(newQueue);
        }
        _created = true;
    }    
}

std::vector<VkSurfaceFormatKHR> EasyVK::LogicalDevice::getSupportedFormats()
{
    uint32_t formatCount;

    vkGetPhysicalDeviceSurfaceFormatsKHR(_my_device.getDevice(), _my_device.getSurface(), &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(_my_device.getDevice(), _my_device.getSurface(), &formatCount, formats.data());
    return formats;
}

std::vector<VkPresentModeKHR> EasyVK::LogicalDevice::getSupportedPresentModes()
{
    uint32_t presentmodesCount;

    vkGetPhysicalDeviceSurfacePresentModesKHR(_my_device.getDevice(), _my_device.getSurface(), &presentmodesCount, nullptr);

    std::vector<VkPresentModeKHR> presentmodes(presentmodesCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(_my_device.getDevice(), _my_device.getSurface(), &presentmodesCount, presentmodes.data());
    return presentmodes;
}

VkSurfaceCapabilitiesKHR EasyVK::LogicalDevice::getCapabilities()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        _my_device.getDevice(), 
        _my_device.getSurface(), 
        &capabilities);
    return capabilities;
}

VkDevice EasyVK::LogicalDevice::getDevice()
{
    return _device;
}

EasyVK::PhysicalDevice EasyVK::LogicalDevice::getPhysicalDevice()
{
    return _my_device;
}

EasyVK::Shader EasyVK::LogicalDevice::createShader(std::vector<char> source)
{
    Shader s(this);
    s.init(source);
    return s;
}

EasyVK::Queue EasyVK::LogicalDevice::getQueue(QueueThat that)
{
    for(auto queue : _savedQueues){
        if((bool)(queue._flags & that)){
            return queue;
        }
    }

    return Queue();
}

EasyVK::Queue EasyVK::LogicalDevice::getPresentQueue()
{
    for(auto queue : _savedQueues){
        if(queue.canPresent){
            return queue;
        }
    }

    return Queue();
}

std::vector<EasyVK::Queue> EasyVK::LogicalDevice::getAllQueues()
{
    return _savedQueues;
}

VkResult EasyVK::LogicalDevice::getResult()
{
    return _result;
}

EasyVK::Framebuffer EasyVK::LogicalDevice::createFrameBuffer(std::vector<ImageView> views, VkExtent2D extent, RenderPass renderPass, uint32_t layers)
{
    Framebuffer fb = Framebuffer(this);
    fb.init(views, extent, renderPass, layers);
    return fb;
}

bool EasyVK::LogicalDevice::ready()
{
    return _created;
}

EasyVK::SwapChain EasyVK::LogicalDevice::createSwapChain(unsigned int frameWidth, unsigned int frameHeight,bool depth_test, VkSurfaceFormatKHR prefered_format, VkPresentModeKHR prefered_presentMode, uint32_t imageArrayLayers, VkImageUsageFlags flags, VkBool32 clipped)
{
    SwapChain sc(this);
    sc.init(frameWidth, frameHeight, prefered_format, prefered_presentMode, imageArrayLayers, flags, clipped, depth_test);
    return sc;
}

EasyVK::GraphicsPipeline EasyVK::LogicalDevice::createGraphicsPipeline(
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages, 
    std::vector<VkDynamicState> dynamicStates, 
    std::vector<VkVertexInputBindingDescription> inputBindings, 
    std::vector<VkVertexInputAttributeDescription> inputAttributes, 
    std::vector<VkViewport> viewports, 
    std::vector<VkRect2D> scissors, 
    VkCullModeFlags cullMode, 
    VkFrontFace frontFace,
    RenderPass renderPass, 
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments,
    std::vector<DescriptorLayout> descriptors, 
    bool depthTestEnable, 
    bool depthWriteEnable, 
    VkCompareOp depthCmpMode, 
    VkPolygonMode polygonMode, 
    VkPrimitiveTopology primitive)
{
    GraphicsPipeline gp(this);

    gp.init(
        shaderStages, 
        dynamicStates,
        inputBindings, 
        inputAttributes, 
        viewports, 
        scissors, 
        cullMode, 
        frontFace,
        renderPass, 
        colorBlendAttachments,
        descriptors,
        depthTestEnable,
        depthWriteEnable,
        depthCmpMode,
        polygonMode,
        primitive
    );

    return gp;
}

uint32_t EasyVK::LogicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags)
{
    VkPhysicalDeviceMemoryProperties mp;
    vkGetPhysicalDeviceMemoryProperties(_my_device.getDevice(), &mp);

    for(uint32_t i = 0; i < mp.memoryTypeCount; i++){
        if((typeFilter & (1 << i)) && (mp.memoryTypes[i].propertyFlags & flags) == flags){
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");
}

EasyVK::Semaphore EasyVK::LogicalDevice::createSemaphore()
{
    return Semaphore(this);
}

EasyVK::Fence EasyVK::LogicalDevice::createFence(bool startSignaled)
{
    return Fence(this, startSignaled);
}

EasyVK::Buffer EasyVK::LogicalDevice::createBuffer(uint64_t size, bool deviceAccesible, VkBufferUsageFlags usage)
{
    return Buffer(this, size, deviceAccesible, usage);
}

EasyVK::Image EasyVK::LogicalDevice::createImage(VkExtent3D extent, VkFormat format, bool cpuAccess, VkImageUsageFlags usage, VkImageType type)
{
    return Image(this, extent, format, usage, type, cpuAccess);
}

EasyVK::DescriptorPool EasyVK::LogicalDevice::createDescriptorPool(uint32_t maxSize, std::vector<VkDescriptorType> types, std::vector<uint32_t> count)
{
    return DescriptorPool(this, maxSize, types, count);
}

void EasyVK::LogicalDevice::waitIdle()
{
    vkDeviceWaitIdle(_device);
}

void EasyVK::LogicalDevice::destroy()
{
    if(_created){
        vkDestroyDevice(_device, nullptr);        
        _created = false;
        _result = VK_NOT_READY;
        _device = VK_NULL_HANDLE;
        _savedQueues = {};
    }
}