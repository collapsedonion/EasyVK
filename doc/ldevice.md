# Device
Device handles all device related features as creating resources and allocating Queues, all queues allocated automatically on device creation, also for all used queue families being used CommandPool will be created. The only valid way to create EasyVK::Device is [EasyVK::PhysicalDevice::createDevice()](pdevice.md#physicaldevicecreatedevice), **do not use the constructor**.

## BufferAccessFlagBits
It is flag bits for BufferAccessFlags bitmask.
```c++
enum BufferAccessFlagBits : uint32_t{
          Graphics = 0b1, //Resource with this flag will have access to Graphics Queue
          Transfer = 0b10, //Resource with this flag will have access to Transfer Queue
          Compute = 0b100, //Resource with this flag will have access to Compute Queue
          Host = 0b1000 //Resource with this flag will have access to Host Proccesor
};
```

## Device::create*CommandBuffer()
Creates [EasyVK::CommandBuffer](cb.md) for corresponding Queue, Queue name comes instead of *'\*'* possible variants is: 
- Graphics 
- Transfer 
- Compute

```c++
EasyVK::CommandBuffer create*CommandBuffer();
```

## Device::createBuffer()
Creates [EasyVK::Buffer](bf.md) with selected size and AccessFlags.

```c++
EasyVK::Buffer createBuffer(
        size_t size, //size of future buffer
        BufferAccessFlags accessFlags //bitmask of BufferAccessFlagBits 
        );
```

## Device::createImage()
Create [EasyVK::Image](img.md) with defined dimensions and color format.

```c++
EasyVK::Image createImage(
        vk::Extent3D size, //dimensions of future image
        vk::Format colorFormat, //format of future image
        BufferAccessFlags accessFlags //bitmask of BufferAccessFlagBits 
        );
```

## Device::createResourceDescriptor()
Create [EasyVK::ResourceDescriptor](rd.md) with defined bindings defined by [EasyVK::ResourceDescriptor::ResourceBindingDescriptor](rd.md#resourcebindingdescriptor).

```c++
EasyVK::ResourceDescriptor createResourceDescriptor(
        const std::map<uint32_t, ResourceDescriptor::ResourceBindingDescriptor> &bindings //first is binding of resource, second is the binding description
        );   
```

## Device::createShader()
Creates [EasyVK::Shader](shader.md) from SPIR-V bytecode.

```c++
EasyVK::Shader createShader(
        const std::vector<uint32_t>& shaderCode //source code of SPIR-V shader
        );
```

## Device::createComputePipeline()
Creates [EasyVK::ComputePipeline](cmppl.md) with selected shader and resource descriptor.

```c++
EasyVK::ComputePipeline createComputePipeline(
        std::pair<Shader, std::string> shader, //shader to use, first is the EasyVK::Shader, second is name of functions defined in it
        ResourceDescriptor descriptor          //EasyVK::ResourceDescriptor that will be used by this pipeline
        );
```

## Device::createRenderPass()
Creates [EasyVK::RenderPass](renderpass.md) render pass with layout of passed image formats, also with allocation at least one FrameBuffer.

```c++
EasyVK::RenderPass createRenderPass(
        const std::vector<Image> images //images to construct render pass layout from
        );   
```

## Device::createSwapChain()
Creates [EasyVK::SwapChain](swapchain.md) for passed vk::SurfaceKHR and at least 2 images or more allocated, works only with enables SWAP_CHAIN feature.

```c++
EasyVK::SwapChain createSwapChain(
        vk::SurfaceKHR surfaceKhr //surface that swap chain will target to
        );
```