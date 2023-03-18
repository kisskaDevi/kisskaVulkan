#include "graphics.h"
#include "../../utils/operations.h"
#include "../../transformational/object.h"
#include "../../transformational/camera.h"

#include <memory>

deferredGraphics::deferredGraphics(){
    outlining.Parent = &base;
    ambientLighting.Parent = &lighting;
}

void deferredGraphics::setMinAmbientFactor(const float& minAmbientFactor)    { ambientLighting.minAmbientFactor = minAmbientFactor;}
void deferredGraphics::setScattering(const bool &enableScattering)           { lighting.enableScattering = enableScattering;}
void deferredGraphics::setTransparencyPass(const bool& transparencyPass)     { base.transparencyPass = transparencyPass;}

void deferredGraphics::destroy()
{
    base.Destroy(&device);
    outlining.DestroyPipeline(&device);
    lighting.Destroy(&device);
    ambientLighting.DestroyPipeline(&device);
    pAttachments.clear();

    filterGraphics::destroy();
}

void deferredGraphics::setAttachments(DeferredAttachments* attachments)
{
    pAttachments.push_back(&attachments->image);
    pAttachments.push_back(&attachments->blur);
    pAttachments.push_back(&attachments->bloom);
    pAttachments.push_back(&attachments->depth);
    pAttachments.push_back(&attachments->GBuffer.position);
    pAttachments.push_back(&attachments->GBuffer.normal);
    pAttachments.push_back(&attachments->GBuffer.color);
    pAttachments.push_back(&attachments->GBuffer.emission);
}

void deferredGraphics::createAttachments(DeferredAttachments* pAttachments)
{
    VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    pAttachments->image.create(&physicalDevice, &device, image.Format, usage, image.Extent, image.Count);
    pAttachments->blur.create(&physicalDevice, &device, image.Format, usage, image.Extent, image.Count);
    pAttachments->bloom.create(&physicalDevice, &device, image.Format, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, image.Extent, image.Count);
    pAttachments->depth.createDepth(&physicalDevice, &device, Image::depthStencilFormat(physicalDevice), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, image.Extent, image.Count);
    pAttachments->GBuffer.position.create(&physicalDevice, &device, VK_FORMAT_R32G32B32A32_SFLOAT, usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, image.Extent, image.Count);
    pAttachments->GBuffer.normal.create(&physicalDevice, &device, VK_FORMAT_R32G32B32A32_SFLOAT, usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, image.Extent, image.Count);
    pAttachments->GBuffer.color.create(&physicalDevice, &device, VK_FORMAT_R8G8B8A8_UNORM, usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, image.Extent, image.Count);
    pAttachments->GBuffer.emission.create(&physicalDevice, &device, VK_FORMAT_R8G8B8A8_UNORM, usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, image.Extent, image.Count);

    VkSamplerCreateInfo SamplerInfo{};
        SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->image.sampler);
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->blur.sampler);
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->bloom.sampler);
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->depth.sampler);
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->GBuffer.position.sampler);
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->GBuffer.normal.sampler);
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->GBuffer.color.sampler);
    vkCreateSampler(device, &SamplerInfo, nullptr, &pAttachments->GBuffer.emission.sampler);
}

void deferredGraphics::createRenderPass()
{
    std::vector<VkAttachmentDescription> attachments;
    attachments.push_back(attachments::imageDescription(pAttachments[attachments.size()]->format));
    attachments.push_back(attachments::imageDescription(pAttachments[attachments.size()]->format));
    attachments.push_back(attachments::imageDescription(pAttachments[attachments.size()]->format));
    attachments.push_back(attachments::depthStencilDescription(pAttachments[attachments.size()]->format));
    attachments.push_back(attachments::imageDescription(pAttachments[attachments.size()]->format));
    attachments.push_back(attachments::imageDescription(pAttachments[attachments.size()]->format));
    attachments.push_back(attachments::imageDescription(pAttachments[attachments.size()]->format));
    attachments.push_back(attachments::imageDescription(pAttachments[attachments.size()]->format));

    std::vector<std::vector<VkAttachmentReference>> attachmentRef;
    attachmentRef.push_back(std::vector<VkAttachmentReference>());
    for(size_t index = 0; index < pAttachments.size() - DeferredAttachments::getGBufferOffset(); index++){
        attachmentRef.back().push_back(VkAttachmentReference{static_cast<uint32_t>(DeferredAttachments::getGBufferOffset() + index), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
    }
    attachmentRef.push_back(std::vector<VkAttachmentReference>());
    for(size_t index = 0; index < DeferredAttachments::getGBufferOffset() - 1; index++){
        attachmentRef.back().push_back(VkAttachmentReference{static_cast<uint32_t>(index), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
    }

    std::vector<std::vector<VkAttachmentReference>> inAttachmentRef;
    inAttachmentRef.push_back(std::vector<VkAttachmentReference>());
    inAttachmentRef.push_back(std::vector<VkAttachmentReference>());
    for(size_t index = 0; index < pAttachments.size() - DeferredAttachments::getGBufferOffset() + 1; index++){
        inAttachmentRef.back().push_back(VkAttachmentReference{static_cast<uint32_t>(DeferredAttachments::getGBufferOffset() - 1 + index), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    }

    std::vector<std::unique_ptr<VkAttachmentReference>> depthAttachmentRef;
    depthAttachmentRef.push_back(
        std::make_unique<VkAttachmentReference>(VkAttachmentReference{static_cast<uint32_t>(DeferredAttachments::getGBufferOffset() - 1), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}));
    depthAttachmentRef.push_back(nullptr);

    std::vector<VkSubpassDescription> subpass;
    auto refIt = attachmentRef.begin();
    auto inRefIt = inAttachmentRef.begin();
    auto depthIt = depthAttachmentRef.begin();
    for(;refIt != attachmentRef.end() && inRefIt != inAttachmentRef.end() && depthIt != depthAttachmentRef.end(); refIt++, inRefIt++, depthIt++){
        subpass.push_back(VkSubpassDescription{});
            subpass.back().pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.back().colorAttachmentCount = static_cast<uint32_t>(refIt->size());
            subpass.back().pColorAttachments = refIt->data();
            subpass.back().inputAttachmentCount = static_cast<uint32_t>(inRefIt->size());
            subpass.back().pInputAttachments = inRefIt->data();
            subpass.back().pDepthStencilAttachment = depthIt->get();
    }

    std::vector<VkSubpassDependency> dependency;
    dependency.push_back(VkSubpassDependency{});
        dependency.back().srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.back().dstSubpass = 0;
        dependency.back().srcStageMask =    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT|
                                            VK_PIPELINE_STAGE_HOST_BIT;
        dependency.back().srcAccessMask =   VK_ACCESS_HOST_READ_BIT;
        dependency.back().dstStageMask =    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT|
                                            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT|
                                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT|
                                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT|
                                            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT|
                                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.back().dstAccessMask =   VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT|
                                            VK_ACCESS_UNIFORM_READ_BIT|
                                            VK_ACCESS_INDEX_READ_BIT|
                                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|
                                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.push_back(VkSubpassDependency{});
        dependency.back().srcSubpass = 0;
        dependency.back().dstSubpass = 1;
        dependency.back().srcStageMask =    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dependency.back().srcAccessMask =   VK_ACCESS_MEMORY_READ_BIT;
        dependency.back().dstStageMask =    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|
                                            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT|
                                            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT|
                                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependency.back().dstAccessMask =   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|
                                            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT|
                                            VK_ACCESS_UNIFORM_READ_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = static_cast<uint32_t>(subpass.size());
        renderPassInfo.pSubpasses = subpass.data();
        renderPassInfo.dependencyCount = static_cast<uint32_t>(subpass.size());
        renderPassInfo.pDependencies = dependency.data();
    vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
}

void deferredGraphics::createFramebuffers()
{
    framebuffers.resize(image.Count);
    for (size_t Image = 0; Image < image.Count; Image++){
        std::vector<VkImageView> attachments;
        for(size_t i=0;i<pAttachments.size();i++){
            attachments.push_back(pAttachments[i]->imageView[Image]);
        }

        VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = image.Extent.width;
            framebufferInfo.height = image.Extent.height;
            framebufferInfo.layers = 1;
        vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[Image]);
    }
}

void deferredGraphics::createPipelines()
{
    base.ExternalPath = externalPath;
    outlining.ExternalPath = externalPath;
    lighting.ExternalPath = externalPath;
    ambientLighting.ExternalPath = externalPath;

    base.createDescriptorSetLayout(&device);
    base.createPipeline(&device,&image,&renderPass);
    outlining.createPipeline(&device,&image,&renderPass);
    lighting.createDescriptorSetLayout(&device);
    lighting.createPipeline(&device,&image,&renderPass);
    ambientLighting.createPipeline(&device,&image,&renderPass);
}

void deferredGraphics::createDescriptorPool()
{
    createBaseDescriptorPool();
    createLightingDescriptorPool();
}

void deferredGraphics::createDescriptorSets()
{
    createBaseDescriptorSets();
    createLightingDescriptorSets();
}

void deferredGraphics::updateDescriptorSets(attachments* depthAttachment, VkBuffer* storageBuffers, size_t sizeOfStorageBuffer, camera* cameraObject)
{
    updateBaseDescriptorSets(depthAttachment, storageBuffers, sizeOfStorageBuffer, cameraObject);
    updateLightingDescriptorSets(cameraObject);
}

void deferredGraphics::updateCommandBuffer(uint32_t frameNumber)
{
    std::vector<VkClearValue> clearValues;
    for(auto& attachments: pAttachments){
        clearValues.push_back(attachments->clearValue);
    }

    VkRenderPassBeginInfo drawRenderPassInfo{};
        drawRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        drawRenderPassInfo.renderPass = renderPass;
        drawRenderPassInfo.framebuffer = framebuffers[frameNumber];
        drawRenderPassInfo.renderArea.offset = {0, 0};
        drawRenderPassInfo.renderArea.extent = image.Extent;
        drawRenderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        drawRenderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[frameNumber], &drawRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        primitiveCount = 0;

        base.render(frameNumber,commandBuffers[frameNumber], primitiveCount);
        outlining.render(frameNumber,commandBuffers[frameNumber]);

    vkCmdNextSubpass(commandBuffers[frameNumber], VK_SUBPASS_CONTENTS_INLINE);

        lighting.render(frameNumber,commandBuffers[frameNumber]);
        ambientLighting.render(frameNumber,commandBuffers[frameNumber]);

    vkCmdEndRenderPass(commandBuffers[frameNumber]);
}

void deferredGraphics::updateObjectUniformBuffer(VkCommandBuffer commandBuffer, uint32_t currentImage)
{
    for(auto& object: base.objects){
        object->updateUniformBuffer(commandBuffer, currentImage);
    }
}

void deferredGraphics::bindBaseObject(object *newObject)
{
    base.objects.push_back(newObject);
}

bool deferredGraphics::removeBaseObject(object* object)
{
    bool result = false;
    for(uint32_t index = 0; index<base.objects.size(); index++){
        if(object==base.objects[index]){
            base.objects.erase(base.objects.begin()+index);
            result = true;
        }
    }
    return result;
}

void deferredGraphics::addLightSource(light* lightSource)
{
    lighting.lightSources.push_back(lightSource);
}

void deferredGraphics::removeLightSource(light* lightSource)
{
    for(uint32_t index = 0; index<lighting.lightSources.size(); index++){
        if(lightSource==lighting.lightSources[index]){
            lighting.lightSources.erase(lighting.lightSources.begin()+index);
        }
    }
}