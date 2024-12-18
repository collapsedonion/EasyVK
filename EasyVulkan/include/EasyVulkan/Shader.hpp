//
// Created by Роман  Тимофеев on 22.04.2024.
//

#ifndef EASY_VULKAN_TEST_SHADER_HPP
#define EASY_VULKAN_TEST_SHADER_HPP
#include <vulkan/vulkan.hpp>
#include <EasyVulkan/Base.hpp>
#include <vector>
#include <cctype>

namespace EasyVK{

    class Shader{

    public:
        ~Shader();

    private:
        vk::Device device;
        vk::ShaderModule module;

    private:
        void setup(vk::Device device, const std::vector<uint32_t>& data);

    private:
        friend Device;
        friend ComputePipeline;
        friend GraphicsPipeline;
    };
}

#endif //EASY_VULKAN_TEST_SHADER_HPP
