//
// Created by Роман  Тимофеев on 24.04.2024.
//

#ifndef EASY_VULKAN_TEST_GRAPHICSPIPELINE_HPP
#define EASY_VULKAN_TEST_GRAPHICSPIPELINE_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/Base.hpp>
#include <EasyVulkan/Shader.hpp>
#include <EasyVulkan/ResourceDescriptor.hpp>
#include <cctype>
#include <vector>
#include <string>

namespace EasyVK{

    class GraphicsPipeline{
    public:
        struct VertexBufferBinding{
            enum AttributeType{
                scalarFloat,
                vec2f,
                vec3f,
                scalarInteger,
                vec2i,
                vec3i
            };

            struct Attribute{
                uint32_t offset;
                uint32_t location;
                AttributeType type;
            };

            uint32_t binding;
            uint32_t  stride;
            bool instanceData = false;
            std::vector<Attribute> attributes;
        };

    public:
        ~GraphicsPipeline();

    private:
        vk::Device device;
        vk::Pipeline pipeline;
        vk::PipelineLayout layout;

    private:
        static std::pair<std::vector<vk::VertexInputBindingDescription>, std::vector<vk::VertexInputAttributeDescription>>
        unwrapVertexBinding(const std::vector<VertexBufferBinding>& vertexBindings);

    protected:
        void setup(
                uint32_t attachmentCount,
                vk::Device device,
                vk::RenderPass renderPass,
                ResourceDescriptor* resourceDescriptor,
                std::pair<Shader*, std::string> vertexShader,
                std::pair<Shader*, std::string> fragmentShader,
                const std::vector<VertexBufferBinding>& bufferBinding,
                vk::PrimitiveTopology topologyType,
                vk::CompareOp depthTestCompareOp = vk::CompareOp::eNever,
                bool counterClockwiseTriangles = false
                );

    private:
        friend RenderPass;
        friend CommandBuffer;
    };
};

#endif //EASY_VULKAN_TEST_GRAPHICSPIPELINE_HPP
