# SwapChain
Holds vk::SwapChain and linked present Queue, used for queue presenting, create only with [EasyVK::Device::createSwapChain](ldevice.md#devicecreateswapchain), **do not use the constructor**.

## SwapChain::getImages()
Returns vector with all linked and allocated Images.

```c++
std::vector<EasyVk::Image> getImages();
```

## SwapChain::getRenderSize()
Returns swap chains image size.

```c++
vk::Extend2D getRenderSize();
```

## SwapChain::nextImage()
Returns id of next target and available image that corresponds index of image at vector returned by [getImages](#swapchaingetimages)

```c++
uint32_t nextImage();
```

## SwapChain::recreate()
Recreates swap chain basing on connected surface capabilities.

```c++
void recreate();
```

## SwapChain::present()
Presents chosen image with linked present Queue, also can be synced with [EasyVK::CommandBuffer](cb.md).

```c++
void present(
        uint32_t imageId, //id of image to present
        const std::vector<CommandBuffer>& awaitFor = {} //optional, CommandBuffers to wait for
        );
```