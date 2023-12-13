#include <EasyVulkan/Instance.hpp>
#include <EasyVulkan/PhysicalDevice.hpp>
#include <EasyVulkan/LogicalDevice.hpp>
#include <EasyVulkan/SwapChain.hpp>
#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/CommandPool.hpp>
#include <EasyVulkan/Shader.hpp>
#include <EasyVulkan/GraphicsPipeline.hpp>
#include <fstream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

std::vector<char> readFile(const std::string& filename){
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open()){
        throw std::runtime_error("failed to open file");
    }
    
    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    
    file.close();
    
    return buffer;
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "EasyVulkanTest", nullptr, nullptr);
    VkSurfaceKHR surface;

    uint32_t glfwExtCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    std::vector<const char*> extensions;
    for(int i = 0; i < glfwExtCount; i++){
        extensions.push_back(glfwExtensions[i]);
    }

    EasyVK::Instance vkInstance(
        "VulkanTest",
        "No engine",
        {1,0,0},
        {0,0,0},
        {1,0,0},
        extensions,
        true
    );

    VkResult result = glfwCreateWindowSurface(vkInstance.getHandledInstance(), window, nullptr, &surface);

    EasyVK::PhysicalDevice physDevice = vkInstance.getPhysicalDevice(
        {EasyVK::QueueThat::GRAPHICS}, 
        {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, 
        true,
        surface);


    EasyVK::LogicalDevice* logDevice = physDevice.createLogicalDevice({
        {EasyVK::QueueThat::GRAPHICS},
        {EasyVK::QueueThat::ZERO, true}
    });

    EasyVK::Queue graphicsQueue = logDevice->getQueue(EasyVK::QueueThat::GRAPHICS);
    EasyVK::Queue presentQueue = logDevice->getPresentQueue();

    EasyVK::SwapChain swapChain = logDevice->createSwapChain(800, 600);
    EasyVK::RenderPass renderPass = swapChain.getRenderPass();

    EasyVK::CommandPool commandPool = graphicsQueue.createCommandPool();

    auto vertShaderSource = readFile("shaders/vert.spv");
    auto fragShaderSource = readFile("shaders/frag.spv");

    EasyVK::Shader vertShader = logDevice->createShader(vertShaderSource);
    EasyVK::Shader fragShader = logDevice->createShader(fragShaderSource);

    VkPipelineColorBlendAttachmentState colorAttachment{};
    colorAttachment.blendEnable = VK_FALSE;
    colorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT;

    EasyVK::GraphicsPipeline graphicsPipeline = logDevice->createGraphicsPipeline(
        {vertShader.getVertexStage("main"), fragShader.getFragmentStage("main")},
        {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
        {},
        {},
        {{0,0,800,600,0.0f,1.0f}},
        {{{0,0},{800,600}}},
        VK_CULL_MODE_BACK_BIT,
        VK_FRONT_FACE_CLOCKWISE,
        renderPass,
        {colorAttachment}
    );

    auto imageAvailable = logDevice->createSemaphore();
    auto renderFence = logDevice->createFence(true);
    auto renderFinished = logDevice->createSemaphore();

    EasyVK::CommandBuffer commandBuffer = commandPool.createCommandBuffer();

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        renderFence.waitForMe(UINT64_MAX);
        auto frameId = swapChain.nextImageId(UINT64_MAX, imageAvailable);
        renderFence.reset();

        commandBuffer.reset();
        commandBuffer.begin();
        commandBuffer.beginRenderPass(
            swapChain.getRenderPass(), 
            swapChain.getFramebuffers()[frameId.second],
            {{0,0}, swapChain.getExtent()},
            {{1.0f,0.0f,0.0f}});
        commandBuffer.bindGraphicsPipeline(graphicsPipeline);
        commandBuffer.setViewports({swapChain.getViewport()});
        commandBuffer.setScissors({{{0,0}, {swapChain.getExtent().height, swapChain.getExtent().width}}});
        commandBuffer.draw(3, 1);
        commandBuffer.endRenderPass();
        commandBuffer.end();

        graphicsQueue.submit({commandBuffer}, {renderFence}, {imageAvailable}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {renderFinished});

        presentQueue.presentSwapChain(swapChain, frameId.second, {renderFinished});
    }

    commandBuffer.destroy();
    graphicsPipeline.destroy();
    vertShader.destroy();
    fragShader.destroy();
    commandPool.destroy();
    swapChain.destroy();
    logDevice->destroy();
    delete logDevice;
    vkInstance.destroy();
}