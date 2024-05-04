#ifndef SELECTOR_H
#define SELECTOR_H

#include "workflow.h"

struct selectorParameters{
    struct{
        std::string storageBuffer;
        std::string position;
        std::string depth;
        std::string transparency;
        std::string defaultDepthTexture;
    }in;
    struct{
        std::string selector;
    }out;
};

class selectorGraphics : public workflow
{
private:
    selectorParameters parameters;

    moon::utils::Attachments frame;
    bool enable{true};

    struct Selector : public workbody{
        void createPipeline(VkDevice device, moon::utils::ImageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device)override;

        uint32_t transparentLayersCount{1};
    }selector;

    void createAttachments(moon::utils::AttachmentsDatabase& aDatabase);
    void createRenderPass();
    void createFramebuffers();
    void createPipelines();
    void createDescriptorPool();
    void createDescriptorSets();
public:
    selectorGraphics(selectorParameters parameters, bool enable, uint32_t transparentLayersCount = 1);

    void destroy() override;
    void create(moon::utils::AttachmentsDatabase& aDatabase) override;
    void updateDescriptorSets(const moon::utils::BuffersDatabase& bDatabase, const moon::utils::AttachmentsDatabase& aDatabase) override;
    void updateCommandBuffer(uint32_t frameNumber) override;
};

#endif // SELECTOR_H
