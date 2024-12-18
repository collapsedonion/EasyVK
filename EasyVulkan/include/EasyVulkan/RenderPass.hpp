//
// Created by Роман  Тимофеев on 24.04.2024.
//

#ifndef EASY_VULKAN_TEST_RENDERPASS_HPP
#define EASY_VULKAN_TEST_RENDERPASS_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/GraphicsPipeline.hpp>
#include <EasyVulkan/Base.hpp>
#include <EasyVulkan/Image.hpp>

namespace EasyVK{
    class RenderPass{

    private:
        struct FrameBufferDescription{
            std::vector<vk::Format> formats;
        };

    public:
        enum class ImageOp{
            Load,
            Clear
        };

    public:
        ~RenderPass();

    private:
        vk::RenderPass renderPass;
        std::vector<vk::Framebuffer> frameBuffer;
        std::map<vk::Framebuffer, vk::Rect2D> renderArea;
        vk::Device device;
        uint32_t imageAttachmentCount = 0;
        std::map<vk::Framebuffer, std::vector<EasyVK::Image::View*>> views;
        std::vector<vk::ClearValue> values;
        FrameBufferDescription myFrameBuffer;

    public:
        void addFrameBuffer(const std::vector<EasyVK::Image*>& images);

        void freeFrameBuffers();

        GraphicsPipeline* createGraphicsPipeline(
                    ResourceDescriptor* resourceDescriptor,
                    std::pair<Shader*, std::string> vertexShader,
                    std::pair<Shader*, std::string> fragmentShader,
                    const std::vector<GraphicsPipeline::VertexBufferBinding>& bufferBinding,
                    vk::PrimitiveTopology topologyType,
                    vk::CompareOp depthTestCompareOp = vk::CompareOp::eNever,
                    bool counterClockwiseTriangles = false
                );

    protected:
        void setup(
                const std::vector<std::pair<EasyVK::Image*, ImageOp>>& images,
                vk::Device device
                );
    private:
        friend Device;
        friend GraphicsPipeline;
        friend CommandBuffer;
    };
}

#endif //EASY_VULKAN_TEST_RENDERPASS_HPP
