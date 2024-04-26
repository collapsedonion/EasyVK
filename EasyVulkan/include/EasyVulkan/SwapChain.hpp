//
// Created by Роман  Тимофеев on 26.04.2024.
//

#ifndef EASY_VULKAN_TEST_SWAPCHAIN_HPP
#define EASY_VULKAN_TEST_SWAPCHAIN_HPP
#include <EasyVulkan/Base.hpp>
#include <EasyVulkan/Image.hpp>
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/CommandBuffer.hpp>
#include <set>

namespace EasyVK {
    class SwapChain :AutoFree{

    public:
        ~SwapChain();

    private:
        vk::PhysicalDevice physicalDevice;
        vk::Device device;
        vk::SurfaceKHR surface;
        std::set<uint32_t> queueFamilies;
        vk::SwapchainKHR swapChain = VK_NULL_HANDLE;
        vk::Extent3D size;
        vk::Format format;
        vk::Queue presentQueue;
        bool seekingForImage = false;

    public:
        std::vector<EasyVK::Image> getImages();
        vk::Extent2D getRenderSize();
        uint32_t nextImage();
        void recreate();
        void present(uint32_t imageId, const std::vector<CommandBuffer>& awaitFor = {});

    protected:
        void setup(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface,
                   std::set<uint32_t> queueFamilies);

    private:
        friend Device;
    };
}
#endif //EASY_VULKAN_TEST_SWAPCHAIN_HPP
