# Resource Descriptor
Holds vk::DescriptorPool and all bindings, create only with [EasyVK::Device::createResourceDescriptor](ldevice.md#devicecreateresourcedescriptor), **do not use the constructor**.

## ResourceTypeBits
Flag bits for ResourceType bitmask.

```c++
enum ResourceTypeBits : uint32_t {
    AUTO = 0b1,
    STORAGE_BUFFER = 0b10,
    UNIFORM_BUFFER = 0b100,
    IMAGE_VIEW = 0b1000,
};
```

## ResourceBindingDescriptor
Descriptor of resource binding, contains reference to DeviceResource and type of resource.

```c++
struct ResourceBindingDescriptor{
    DeviceResource& resource; //reference to DeviceResource
    DeviceResource::ResourceType type = DeviceResource::AUTO; //binding type optional
};
```

## ResourceDescriptor::updateBinding()
Updates previously allocated binding does not allocate new one.

```c++
void updateBinding(
        uint32_t binding, //id of modified binding
        DeviceResource& resource //reference to Device resource
        );
```