//
// Created by Роман  Тимофеев on 22.04.2024.
//

#ifndef EASY_VULKAN_TEST_IMAGE_HPP
#define EASY_VULKAN_TEST_IMAGE_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/Base.hpp>
#include <EasyVulkan/DeviceResourceInterface.hpp>

namespace EasyVK{

    class Device;

    class Image{

    public:
        class View : public DeviceResource{
        public:
            ~View();

        private:
            vk::Device device;
            vk::ImageView view;
            vk::Format format;
            vk::Sampler sampler;

        protected:
            void setup(vk::Device device, vk::Format colorFormat, vk::Extent3D size, vk::Image image);
            virtual bool checkResourceTypeCompatability(ResourceType type) override;
            virtual void bindToDescriptorSet(vk::Device device, vk::DescriptorSet set, ResourceType type, uint32_t binding) override;
            virtual ResourceType getVerifiedResourceType(ResourceType type) override;

        private:
            friend RenderPass;
            friend Image;
        };

    public:
        ~Image();

    private:
        vk::Device device;
        vk::DeviceMemory allocatedMemory;
        vk::Image image;
        vk::Format colorFormat;
        vk::Extent3D size;
        uint32_t allocatedSize;
        vk::ImageTiling tiling;
        vk::ImageLayout layout;
        bool createdFromSwapChain = false;

    public:
        void* bind();
        void unbind();
        vk::Format getImageFormat();
        View* getView();

    private:
        void createFromSwapChain(vk::Device device, vk::Format format, vk::Extent3D size, vk::Image image);
        void allocate(vk::PhysicalDevice physicalDevice, vk::Device device, vk::Format colorFormat, vk::Extent3D size, const std::vector<uint32_t> &queueFamilies, bool hostAccess);

    private:
        friend Device;
        friend SwapChain;
        friend RenderPass;
        friend CommandBuffer;
    };
}

#endif //EASY_VULKAN_TEST_IMAGE_HPP
