# Render pass
Holds vk::RenderPass, also holds and allocates vk::Framebuffer objects, on creation allocates one framebuffer, create only [EasyVK::Device::createRenderPass](ldevice.md#devicecreaterenderpass), **do not use the constructor**.

## RenderPass::freeFrameBuffers()
Deallocates all linked frame buffers.

```c++
void freeFrameBuffers();
```

## RenderPass::addFrameBuffer()
Allocates new frame buffer and appends to list, new frame buffer should have the same layout as previous.

```c++
void addFrameBuffer(
        const std::vector<EasyVK::Image>& images //image that will be linked to new frame buffer
        );   
```

## RenderPass::createGraphicsPipeline()
Create graphics pipeline with passed arguments, always with dynamic viewport and scissors.

```c++
EasyVK::GraphicsPipeline createGraphicsPipeline(
        ResourceDescriptor resourceDescriptor, //resource descriptor to bind to pipeline
        std::pair<Shader, std::string> vertexShader, //shader and function name of vertex shader
        std::pair<Shader, std::string> fragmentShader, //shader and function name of fragment shader
        std::vector<GraphicsPipeline::VertexBufferBinding> bufferBinding, //vertex buffer binding for graphics pipeline
        vk::PrimitiveTopology topologyType, //type of topology to render
        vk::CompareOp depthTestCompareOp = vk::CompareOp::eNever, //depth test compare mode
        bool counterClockwiseTriangles = false //mode of triangle forward face detection
        );
```