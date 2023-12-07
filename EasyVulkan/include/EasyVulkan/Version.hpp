#pragma once
#include <vulkan/vulkan.h>

namespace EasyVK{

struct Version{ 
    int major;
    int minor;
    int patch;

    inline int getVulkanVersion(bool api = false) const{
        return VK_MAKE_API_VERSION(0, major, minor, api ?  0 : patch);
    }
};

}