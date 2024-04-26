#include <fstream>
#include <set>
#include <EasyVulkan/Instance.hpp>
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define EASYVK_GLFW_HELPER
#include <EasyVulkan/GLFWHelper.hpp>
#include <EasyMatrix/matricies.hpp>
#include <EasyMatrix/vectors.hpp>

std::vector<uint32_t> readFile(const std::string& filename){
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open()){
        throw std::runtime_error("failed to open file");
    }
    
    size_t fileSize = file.tellg();
    std::vector<uint32_t> buffer(fileSize);
    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    
    file.close();
    
    return buffer;
}

struct Vertex{
    EasyMatrix::Vector3f position;
    EasyMatrix::Vector3f color;
};

std::vector<Vertex> vertexBufferContent = {
        {-0.5, -0.5, 1.0, 1.0, 0.0, 0.0},
        {-0.5, 0.5, 1.0, 0.0, 1.0, 0.0},
        {0.5, -0.5, 1.0, 0.0, 0.0, 1.0},
        {-0.5, 0.5, 1.0, 0.0, 1.0, 0.0},
        {0.5, 0.5, 1.0, 0.0, 0.0, 0.0},
        {0.5, -0.5, 1.0, 0.0, 0.0, 1.0}
};

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "EasyVulkanTest", nullptr, nullptr);
    VkSurfaceKHR surface;

    uint32_t glfwExtCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    std::set<std::string> extensions;
    for(int i = 0; i < glfwExtCount; i++){
        extensions.insert(std::string(glfwExtensions[i]));
    }

    EasyVK::Instance instance = EasyVK::Instance(
            "TestApp",
            EasyVK::Instance::V1_0,
            {"VK_LAYER_KHRONOS_validation"},
            extensions);

    surface = EasyVK::GLFWHelper::getGLFWSurface(window, instance);

    EasyVK::PhysicalDevice* physicalDevice;
    try {
        physicalDevice = instance.getPhysicalDevice(EasyVK::PhysicalDevice::DISCRETE,
                                                    EasyVK::GRAPHICS | EasyVK::SWAP_CHAIN);
    }catch (...){
        physicalDevice = instance.getPhysicalDevice(EasyVK::PhysicalDevice::INTEGRATED,
                                                    EasyVK::GRAPHICS | EasyVK::SWAP_CHAIN);
    }

    auto vertexSource = readFile("/Users/romantimofeev/Documents/CodingProjects/EasyVulkan/test/shaders/vert.spv");
    auto fragmentSource = readFile("/Users/romantimofeev/Documents/CodingProjects/EasyVulkan/test/shaders/frag.spv");

    EasyVK::Device device = physicalDevice->createDevice(surface);
    EasyVK::SwapChain swapChain = device.createSwapChain(surface);

    std::vector<EasyVK::Image> images = swapChain.getImages();

    EasyVK::Shader vertexShader = device.createShader(vertexSource);
    EasyVK::Shader fragmentShader = device.createShader(fragmentSource);

    EasyVK::Buffer buffer = device.createBuffer(sizeof(EasyMatrix::Matrix4x4f), EasyVK::Device::Graphics | EasyVK::Device::Host);
    EasyVK::Buffer vertexBuffer = device.createBuffer(sizeof(Vertex) * vertexBufferContent.size(), EasyVK::Device::Graphics | EasyVK::Device::Host);

    void* pVertexBuffer = vertexBuffer.bind();
    memcpy(pVertexBuffer, vertexBufferContent.data(), vertexBufferContent.size() * sizeof (Vertex));
    vertexBuffer.unbind();

    EasyVK::ResourceDescriptor resourceDescriptor = device.createResourceDescriptor({{0,{buffer, EasyVK::DeviceResource::STORAGE_BUFFER}}});
    EasyVK::RenderPass renderPass = device.createRenderPass({images[0]});

    for(auto image = images.begin() + 1; image != images.end(); image++){
        renderPass.addFrameBuffer({*image});
    }

    EasyVK::GraphicsPipeline::VertexBufferBinding vertexBufferBinding;
    vertexBufferBinding.stride = sizeof(Vertex);
    vertexBufferBinding.binding = 0;
    vertexBufferBinding.attributes = {
            {0, 0, EasyVK::GraphicsPipeline::VertexBufferBinding::vec3f},
            {offsetof(Vertex, color), 1, EasyVK::GraphicsPipeline::VertexBufferBinding::vec3f}
    };

    EasyVK::GraphicsPipeline graphicsPipeline = renderPass.createGraphicsPipeline(
            resourceDescriptor,
            {vertexShader, "main"},
            {fragmentShader, "main"},
            {vertexBufferBinding},
            vk::PrimitiveTopology::eTriangleList
            );

    EasyVK::CommandBuffer commandBuffer = device.createGraphicsCommandBuffer();

    auto* bufferMem = (EasyMatrix::Matrix4x4f*)buffer.bind();

    double prevTime = glfwGetTime();
    double deltaTime = 0;
    double degrees = 0;

    while (!glfwWindowShouldClose(window)){
        vk::Extent2D renderSize = swapChain.getRenderSize();
        double currentTime = glfwGetTime();
        deltaTime = currentTime - prevTime;
        prevTime = currentTime;
        degrees += deltaTime * 180;
        *bufferMem = EasyMatrix::createRotationMatrixZ(EasyMatrix::degreesToRadians((float)degrees)) * EasyMatrix::createPerspectiveProjection(90.0f, (float)renderSize.width, (float)renderSize.height, 0.1, 100);

        glfwPollEvents();
        uint32_t imageId = swapChain.nextImage();
        commandBuffer.reset();
        commandBuffer.begin();
        commandBuffer.setViewport({vk::Viewport(0,0, (float)renderSize.width, (float)renderSize.height)});
        commandBuffer.setScissors({vk::Rect2D({},renderSize)});
        commandBuffer.beginRenderPass(renderPass, imageId);
        commandBuffer.bindVertexBuffer(vertexBuffer);
        commandBuffer.bind(graphicsPipeline);
        commandBuffer.bind(graphicsPipeline, resourceDescriptor);
        commandBuffer.draw(6, 1);
        commandBuffer.endRenderPass();
        commandBuffer.end();

        commandBuffer.submit();
        commandBuffer.await();
        swapChain.present(imageId, {commandBuffer});
    }

    return 0;
}
