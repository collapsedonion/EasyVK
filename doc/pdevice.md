# Physical Device

PhysicalDevice holds selected and configured vk::PhysicalDeice resource the only valid way to create it is [EasyVK::Instance::getPhysicalDevice()](instance.md#instancegetphysicaldevice) **Do not use the constructor**.

## PhysicalDevice::CreateDevice()
Creates an Instance of [EasyVK::Device](ldevice.md) that is support all features of this EasyVK::PhysicalDevice.

```c++
EasyVK::Device createDevice(
        vk::SurfaceKHR surface = VK_NULL_HANDLE //optional but required when SWAP_CHAIN feature enabled
        );   
```