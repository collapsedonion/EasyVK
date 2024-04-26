//
// Created by Роман  Тимофеев on 23.04.2024.
//

#ifndef EASY_VULKAN_TEST_BASE_HPP
#define EASY_VULKAN_TEST_BASE_HPP

namespace EasyVK{
    class Device;
    class Shader;
    class ComputePipeline;
    class Buffer;
    class CommandBuffer;
    class RenderPass;
    class GraphicsPipeline;
    class GLFWHelper;
    class SwapChain;

    class AutoFree{

    protected:
        int* referenceCounter;

    public:
        AutoFree();
        AutoFree(const AutoFree& ref);
        AutoFree(AutoFree&& ref) noexcept;

        AutoFree& operator = (const AutoFree& other);

        AutoFree& operator = (AutoFree&& other) noexcept;

    protected:
        bool isKilled();

    public:
        ~AutoFree();
    };
};

#endif //EASY_VULKAN_TEST_BASE_HPP
