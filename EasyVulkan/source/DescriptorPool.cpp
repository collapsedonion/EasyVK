#include <EasyVulkan/DescriptorPool.hpp>
#include <EasyVulkan/LogicalDevice.hpp>

EasyVK::DescriptorPool::DescriptorPool(LogicalDevice *device, uint32_t maxSize, std::vector<VkDescriptorType> types, std::vector<uint32_t> count)
{

    _device = device;
    std::vector<VkDescriptorPoolSize> sizes{};
    for(size_t i = 0; i < types.size(); i++){
        VkDescriptorPoolSize ps{};
        ps.type = types[i];
        ps.descriptorCount = count.size() > i ? count[i] : 1;
        sizes.push_back(ps);
    }

    VkDescriptorPoolCreateInfo poolCI{};
    poolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCI.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCI.pPoolSizes = sizes.data();
    poolCI.poolSizeCount = sizes.size();
    poolCI.maxSets = maxSize;
    
    _sets.resize(maxSize);

    _result = vkCreateDescriptorPool(_device->getDevice(), &poolCI, nullptr, &_pool);
}

EasyVK::DescriptorPool::DescriptorPool()
{
}

VkResult EasyVK::DescriptorPool::getResult()
{
    return _result;
}

bool EasyVK::DescriptorPool::isReady()
{
    return _result == VK_SUCCESS;
}

VkDescriptorPool EasyVK::DescriptorPool::getPool()
{
    return _pool;
}

VkResult EasyVK::DescriptorPool::allocateAtIndex(long long i, VkDescriptorSetLayout layout)
{
    if(i < _sets.size() && _sets[i] == VK_NULL_HANDLE){
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorSetCount = 1;
        allocInfo.descriptorPool = _pool;
        allocInfo.pSetLayouts = &layout;

        return vkAllocateDescriptorSets(_device->getDevice(), &allocInfo, &_sets[i]);
    }

    return VK_ERROR_UNKNOWN;
}

void EasyVK::DescriptorPool::configureAtIndex(long long i, uint32_t binding, VkDescriptorType type, Buffer buffer, Image image, uint32_t sampler_id, uint32_t arrayElem)
{
    if(i < _sets.size() && _sets[i] != VK_NULL_HANDLE){
        VkWriteDescriptorSet writeInfo{};
        writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeInfo.dstSet = _sets[i];
        writeInfo.dstArrayElement = arrayElem;
        writeInfo.dstBinding = binding;
        writeInfo.descriptorType = type;
        writeInfo.descriptorCount = 1;
        VkDescriptorBufferInfo bI{};
        VkDescriptorImageInfo iI{};

        if(buffer.isReady()){
            bI.buffer = buffer.getBuffer();
            bI.offset = 0;
            bI.range = VK_WHOLE_SIZE;
            writeInfo.pBufferInfo = &bI;
        }else if(image.isReady()){
            iI.imageView = image.getImageView().getImageView();
            iI.sampler = image.getSampler(sampler_id);
            iI.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            writeInfo.pImageInfo = &iI;
        }

        vkUpdateDescriptorSets(_device->getDevice(), 1, &writeInfo, 0, nullptr);
    }
}

void EasyVK::DescriptorPool::freeAtIndex(long long i)
{
    if(i < _sets.size() && _sets[i] != VK_NULL_HANDLE){
        vkFreeDescriptorSets(_device->getDevice(), _pool, 1, &_sets[i]);
        _sets[i] = VK_NULL_HANDLE;
    }
}

VkDescriptorSet EasyVK::DescriptorPool::getDescriptor(long long i)
{
    return  i < _sets.size() ? _sets[i] : VK_NULL_HANDLE;
}

void EasyVK::DescriptorPool::destroy()
{
    if(isReady()){
        vkDestroyDescriptorPool(_device->getDevice(), _pool, nullptr);
        _result = VK_NOT_READY;
    }
}
