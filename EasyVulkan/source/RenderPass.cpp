#include <EasyVulkan/RenderPass.hpp>
#include <EasyVulkan/LogicalDevice.hpp>


EasyVK::RenderPass::RenderPass(LogicalDevice *device)
{
    _device = device;
}

EasyVK::RenderPass::RenderPass()
{
}

void EasyVK::RenderPass::init(std::vector<VkFormat> attachment_format, VkFormat stencil_format, bool stencil_attachment)
{
    if(_device == nullptr){
        return;
    }

    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkAttachmentReference> attachment_references;
    VkAttachmentReference depth_att;

    for(auto elem : attachment_format){
        VkAttachmentDescription ad{};
        ad.format = elem;
        ad.samples = VK_SAMPLE_COUNT_1_BIT;
        ad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ad.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        ad.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ad.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        ad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments.push_back(ad);

        VkAttachmentReference ar;
        ar.attachment = attachment_references.size();
        ar.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment_references.push_back(ar);
    }

    if(stencil_attachment){
        VkAttachmentDescription ad{};
        ad.format = stencil_format;
        ad.samples = VK_SAMPLE_COUNT_1_BIT;
        ad.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ad.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ad.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ad.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ad.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ad.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments.push_back(ad);
        
        VkAttachmentReference ar;
        ar.attachment = attachment_references.size();
        ar.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depth_att = ar;
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = attachment_references.size();
    subpass.pColorAttachments = attachment_references.data();

    if(stencil_attachment){
        subpass.pDepthStencilAttachment = &depth_att;
    }

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = 0;
    
    if(stencil_attachment == false){
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }else{
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; 
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        subpass.pDepthStencilAttachment = &depth_att;
    }

    VkRenderPassCreateInfo rpci{};
    rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpci.attachmentCount = attachments.size();
    rpci.pAttachments = attachments.data();
    rpci.pDependencies = &dependency;
    rpci.dependencyCount = 1;
    rpci.subpassCount = 1;
    rpci.pSubpasses = &subpass;
    
    _result = vkCreateRenderPass(_device->getDevice(), &rpci, nullptr, &_renderpass);
    _ready = _result == VK_SUCCESS;
}

VkRenderPass EasyVK::RenderPass::getRenderPass()
{
    return _renderpass;
}

EasyVK::LogicalDevice EasyVK::RenderPass::getDevice()
{
    return *_device;
}

VkResult EasyVK::RenderPass::getResult()
{
    return _result;
}

bool EasyVK::RenderPass::isReady()
{
    return _ready;
}

void EasyVK::RenderPass::destroy()
{
    if(_ready){
        _ready = false;
        _result = VK_NOT_READY;
        vkDestroyRenderPass(_device->getDevice(), _renderpass, nullptr);
        _renderpass = VK_NULL_HANDLE;
    }
}