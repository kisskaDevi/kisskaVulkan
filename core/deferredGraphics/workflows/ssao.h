#ifndef SSAO_H
#define SSAO_H

#include "workflow.h"

struct SSAOParameters{
    struct{
        std::string camera;
        std::string position;
        std::string normal;
        std::string color;
        std::string depth;
        std::string defaultDepthTexture;
    }in;
    struct{
        std::string ssao;
    }out;
};

class SSAOGraphics : public workflow
{
private:
    SSAOParameters parameters;

    attachments frame;
    bool enable{true};

    struct SSAO : public workbody{
        void createPipeline(VkDevice device, imageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device)override;
    }ssao;

    void createAttachments(attachmentsDatabase& aDatabase);
    void createRenderPass();
    void createFramebuffers();
    void createPipelines();
    void createDescriptorPool();
    void createDescriptorSets();
public:
    SSAOGraphics(SSAOParameters parameters, bool enable);

    void destroy() override;
    void create(attachmentsDatabase& aDatabase) override;
    void updateDescriptorSets(const buffersDatabase& bDatabase, const attachmentsDatabase& aDatabase) override;
    void updateCommandBuffer(uint32_t frameNumber) override;
};
#endif // SSAO_H
