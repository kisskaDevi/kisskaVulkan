#ifndef SSAO_H
#define SSAO_H

#include "filtergraphics.h"

class camera;

class SSAOGraphics : public filterGraphics
{
private:
    VkRenderPass                        renderPass{VK_NULL_HANDLE};
    std::vector<VkFramebuffer>          framebuffers;

    struct SSAO : public filter{
        std::string                     vertShaderPath;
        std::string                     fragShaderPath;

        void createPipeline(VkDevice device, imageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device)override;
    }ssao;

public:
    SSAOGraphics();
    void destroy() override;
    void freeCommandBuffer(VkCommandPool commandPool){
        if(commandBuffers.data()){
            vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        }
        commandBuffers.resize(0);
    }

    void createAttachments(uint32_t attachmentsCount, attachments* pAttachments) override;
    void createRenderPass() override;
    void createFramebuffers() override;
    void createPipelines() override;

    void createDescriptorPool() override;
    void createDescriptorSets() override;
    void updateDescriptorSets(camera* cameraObject, DeferredAttachments deferredAttachments);

    void updateCommandBuffer(uint32_t frameNumber) override;
};
#endif // SSAO_H
