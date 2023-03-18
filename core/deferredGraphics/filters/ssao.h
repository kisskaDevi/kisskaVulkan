#ifndef SSAO_H
#define SSAO_H

#include "filtergraphics.h"

class camera;

class SSAOGraphics : public filterGraphics
{
private:
    struct SSAO : public filter{
        void createPipeline(VkDevice device, imageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device)override;
    }ssao;

public:
    SSAOGraphics() = default;
    void destroy();

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