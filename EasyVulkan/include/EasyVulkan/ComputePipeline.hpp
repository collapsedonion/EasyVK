//
// Created by Роман  Тимофеев on 23.04.2024.
//

#ifndef EASY_VULKAN_TEST_COMPUTEPIPELINE_HPP
#define EASY_VULKAN_TEST_COMPUTEPIPELINE_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/Shader.hpp>
#include <EasyVulkan/ResourceDescriptor.hpp>
#include <EasyVulkan/Base.hpp>
#include <string>

namespace EasyVK{
    class Device;

    class ComputePipeline{

    public:
        ~ComputePipeline();

    private:
        vk::Device device;
        vk::Pipeline pipeline;
        vk::PipelineLayout layout;

    protected:
        void setup(std::pair<Shader*, std::string> shader, ResourceDescriptor* resourceDescriptor, vk::Device device);

    private:
        friend Device;
        friend CommandBuffer;
    };
}

#endif //EASY_VULKAN_TEST_COMPUTEPIPELINE_HPP
