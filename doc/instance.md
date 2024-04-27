# Instance

EasyVK::Instance is a handler for vk::Instance it's manages extension support also is used for creating the physical device.

# Overview
## VULKAN_VERSION
Enumeration which is used to define target Vulkan version.
```c++
enum VULKAN_VERSION{
    V1_0
};
```
## FeaturesBits
Enumeration with bits for EasyVK::Feature bitmask which is used to define features that will be supported by device.
```c++
enum FeaturesBits : uint32_t{
    COMPUTE = 0b1, //Device will support compute pipeline
    GRAPHICS = 0b10, //Device will support graphics pipeline
    SWAP_CHAIN = 0b100//Device can create a SwapChain
};
```

## PhysicalDevice::DeviceType
Enumeration which is used to select graphics card type.

```c++
enum DeviceTypeBits{
    ANY, //Returns first founded gpu with desired features
    DISCRETE, //Returns first founded discrete gpu with desired features
    INTEGRATED //Returns first founded integrated gpu with desired features
};
```

## Instance()
Default constructor used to create Instance

```c++        
Instance(
        const std::string& application_name, //name of created instance
        VULKAN_VERSION version,              //target vulkan api version
        const std::set<std::string>& supported_layers, //validation layers that should be enabled at instance
        const std::set<std::string>& supported_extensions //extension that should be enabled at instance
        );
```
## Instance::layerEnabled()
Returns true if given layer enabled at selected instance
```c++
bool layerEnabled(
        const std::string& name //name of checked layer
        );
```

## Instance::extensionEnabled()
Returns true if given extension enabled at selected instance
```c++
bool extensionEnabled(
        const std::string& name //name of checked extension
        );
```

## Instance::getPhysicalDevice()
Search's for suitable physical device and return pointer to created [EasyVK::PhysicalDevice](pdevice.md) object. Throws a runtime_error if such device does not exist.
```c++
EasyVK::PhysicalDevice* getPhysicalDevice(
        EasyVK::PhysicalDevice::DeviceType type, //type of device to search
        EasyVK::Features features               //bitmask of features needed to be enabled at created device
        );  
```