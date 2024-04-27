# EasyVK
EasyVK is the abstraction for Vulkan API graphics library, that cares most of the routine work like memory allocation and vulkan object creation.

# Main objective
The main objective of EasyVK is to simplify Vulkan API to make more work in less of code. Most of the information for resource creation will be found automatically and resources with unnecessary direct access will be hidden in other objects.

# Short Example Of Creating a Buffer
```c++
#include <EasyVulkan/EasyVK.hpp>

int main(){
    //Creating Vulkan Instance
    EasyVK::Instance instance = EasyVK::Instance(
            "Short Example",
            EasyVK::Instance::V1_0,
            {},
            {}
            );

    //Acquiring physical device with compute feature
    EasyVK::PhysicalDevice* physicalDevice = instance.getPhysicalDevice(
            EasyVK::PhysicalDevice::ANY,
            EasyVK::FeaturesBits::COMPUTE);

    //Creating logical device
    EasyVK::Device device = physicalDevice->createDevice();

    //Creating buffer with size of 256 byte, visible by host and compute shader
    EasyVK::Buffer buffer = device.createBuffer(256, EasyVK::Device::Host | EasyVK::Device::Compute);


    //Buffer and device will be freed automatically
    return 0;
}
```

# Bare Vulkan Compatability
Compatability with bare vulkan code is planed for next updates.

# [Documentation](doc/docs.md)