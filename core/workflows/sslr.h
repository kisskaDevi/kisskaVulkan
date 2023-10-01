#ifndef SSLR_H
#define SSLR_H

#include "workflow.h"

class camera;

class SSLRGraphics : public workflow
{
private:
    struct SSLR : public workbody{
        void createPipeline(VkDevice device, imageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device) override;
    }sslr;

public:
    SSLRGraphics() = default;
    void destroy();

    void createAttachments(uint32_t attachmentsCount, attachments* pAttachments);
    void createRenderPass() override;
    void createFramebuffers() override;
    void createPipelines() override;

    void createDescriptorPool() override;
    void createDescriptorSets() override;
    void updateDescriptorSets(camera* cameraObject, attachments* position, attachments* normal, attachments* image, attachments* depth,
        attachments* layerPosition, attachments* layerNormal, attachments* layerImage, attachments* layerDepth);

    void updateCommandBuffer(uint32_t frameNumber) override;
};

#endif // SSLR_H