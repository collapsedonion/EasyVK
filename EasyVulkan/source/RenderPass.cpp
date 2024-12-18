//
// Created by Роман  Тимофеев on 24.04.2024.
//
#include <EasyVulkan/RenderPass.hpp>

void EasyVK::RenderPass::setup(const std::vector<std::pair<EasyVK::Image*, ImageOp>>& images,
                               vk::Device device) {
    std::vector<vk::AttachmentDescription> attachments;
    std::vector<vk::ImageView> viewAttachments;

    uint32_t minWidth = UINT32_MAX;
    uint32_t minHeight = UINT32_MAX;
    std::vector<vk::AttachmentReference> attachmentReferences;
    vk::AttachmentReference depthAttachment;
    bool depthAttachmentEnable = false;

    long long attachmentId = 0;

    std::vector<EasyVK::Image::View*> views;

    this->myFrameBuffer = {};

    for(auto imageSettings : images){
        auto image = imageSettings.first; 
        vk::AttachmentDescription createInfo = {};
        createInfo.format = image->colorFormat;

        switch (imageSettings.second)
        {
        case ImageOp::Clear:
            createInfo.loadOp = vk::AttachmentLoadOp::eClear;
            createInfo.initialLayout = vk::ImageLayout::eUndefined;
            break;
        case ImageOp::Load:
            createInfo.loadOp = vk::AttachmentLoadOp::eLoad;
            createInfo.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
        
        default:
            break;
        }

        createInfo.stencilStoreOp = vk::AttachmentStoreOp::eStore;
        createInfo.stencilLoadOp = vk::AttachmentLoadOp::eClear;
        createInfo.storeOp = vk::AttachmentStoreOp::eStore;

        vk::AttachmentReference attachmentReference = {};
        attachmentReference.attachment = attachmentId;
        attachmentId++;

        if(image->colorFormat == vk::Format::eD32SfloatS8Uint ||
           image->colorFormat == vk::Format::eD32Sfloat ||
           image->colorFormat == vk::Format::eD24UnormS8Uint ||
           image->colorFormat == vk::Format::eD16UnormS8Uint ||
           image->colorFormat == vk::Format::eD16Unorm){
            vk::ClearValue clearValue;
            clearValue.depthStencil = vk::ClearDepthStencilValue(1, 0);
            values.push_back(clearValue);
            createInfo.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            if(imageSettings.second == ImageOp::Load){
                createInfo.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            }

            attachmentReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment = attachmentReference;
            depthAttachmentEnable = true;
        }else{
            vk::ClearValue clearValue;
            clearValue.color = vk::ClearColorValue(0,0,0,1);
            values.push_back(clearValue);
            imageAttachmentCount++;
            createInfo.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
            attachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
            attachmentReferences.push_back(attachmentReference);
        }

        attachments.push_back(createInfo);
        views.push_back(image->getView());
        viewAttachments.push_back(views.back()->view);

        this->myFrameBuffer.formats.push_back(image->colorFormat);

        if(image->size.width < minWidth){
            minWidth = image->size.width;
        }
        if(image->size.height < minHeight){
            minHeight = image->size.height;
        }
    }

    vk::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = attachmentReferences.size();
    subpass.pColorAttachments = attachmentReferences.data();

    vk::SubpassDependency dependency = {};
    dependency.srcSubpass = vk::SubpassExternal;
    dependency.dstSubpass = 0;

    if(depthAttachmentEnable){
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        subpass.pDepthStencilAttachment = &depthAttachment;
    }else{
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    }

    vk::RenderPassCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    renderPass = device.createRenderPass(createInfo);

    vk::FramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
    framebufferCreateInfo.renderPass = this->renderPass;
    framebufferCreateInfo.attachmentCount = viewAttachments.size();
    framebufferCreateInfo.pAttachments = viewAttachments.data();
    framebufferCreateInfo.width = minWidth;
    framebufferCreateInfo.height = minHeight;
    framebufferCreateInfo.layers = 1;

    this->frameBuffer.push_back(device.createFramebuffer(framebufferCreateInfo));
    this->views.emplace(this->frameBuffer.back(), views);
    this->renderArea.emplace(this->frameBuffer.back(), vk::Rect2D({}, {minWidth, minHeight}));
    this->device = device;
}

EasyVK::RenderPass::~RenderPass() {
    for(auto elem : frameBuffer) {
        device.destroy(elem);
    }
    device.destroy(renderPass);
}

EasyVK::GraphicsPipeline* EasyVK::RenderPass::createGraphicsPipeline(EasyVK::ResourceDescriptor* resourceDescriptor,
                                                                     std::pair<Shader*, std::string> vertexShader,
                                                                     std::pair<Shader*, std::string> fragmentShader,
                                                                     const std::vector<GraphicsPipeline::VertexBufferBinding>& bufferBinding,
                                                                     vk::PrimitiveTopology topologyType,
                                                                     vk::CompareOp depthTestCompareOp,
                                                                     bool counterClockwiseTriangles) {
    auto graphicsPipeline = new GraphicsPipeline();
    graphicsPipeline->setup(this->imageAttachmentCount, device, renderPass, resourceDescriptor, vertexShader, fragmentShader, bufferBinding, topologyType, depthTestCompareOp, counterClockwiseTriangles);
    return graphicsPipeline;
}

void EasyVK::RenderPass::addFrameBuffer(const std::vector<EasyVK::Image*> &images) {

    std::vector<EasyVK::Image::View*> views;
    std::vector<vk::ImageView> nativeViews;
    vk::Extent2D size;
    size.width = UINT32_MAX;
    size.height = UINT32_MAX;

    for(size_t i = 0; i < images.size(); i++){
        auto image = images[i];
        views.push_back(image->getView());
        nativeViews.push_back(views.back()->view);

        if(this->myFrameBuffer.formats[i] != image->colorFormat){
            throw std::runtime_error("Invalid image format");
        }

        if(size.width > image->size.width){
            size.width = image->size.width;
        }

        if(size.height > image->size.height){
            size.height = image->size.height;
        }
    }

    vk::FramebufferCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eFramebufferCreateInfo;
    createInfo.renderPass = this->renderPass;
    createInfo.attachmentCount = nativeViews.size();
    createInfo.pAttachments = nativeViews.data();
    createInfo.height = size.height;
    createInfo.width = size.width;
    createInfo.layers = 1;

    this->frameBuffer.push_back(device.createFramebuffer(createInfo));
    this->renderArea.emplace(this->frameBuffer.back(), vk::Rect2D({}, size));
    this->views.emplace(this->frameBuffer.back(), views);
}

void EasyVK::RenderPass::freeFrameBuffers() {
    renderArea.clear();

    for(auto view : views){
        for(auto v : view.second){
            delete v;
        }
    }

    for(auto elem : frameBuffer){
        device.destroy(elem);
    }

    frameBuffer.clear();
}
