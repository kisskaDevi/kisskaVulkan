#ifndef CUSTOMFILTER_H
#define CUSTOMFILTER_H

#include "workflow.h"

struct CustomFilterPushConst{
    alignas (4) float deltax;
    alignas (4) float deltay;
    alignas (4) float blitFactor;
};

class customFilter : public workflow
{
private:
    std::vector<attachments> frames;
    attachments bufferAttachment;
    attachments* srcAttachment{nullptr};

    bool enable{true};
    float blitFactor{0.0f};
    float xSamplerStep{1.5f};
    float ySamplerStep{1.5f};

    struct Filter : public workbody{
        void createPipeline(VkDevice device, imageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device) override;
    }filter;

    struct Bloom : public workbody{
        void createPipeline(VkDevice device, imageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device) override;

        uint32_t blitAttachmentsCount{0};
    }bloom;

    void render(VkCommandBuffer commandBuffer, attachments image, uint32_t frameNumber, uint32_t framebufferIndex, workbody* worker);

    void createAttachments(std::unordered_map<std::string, std::pair<bool,std::vector<attachments*>>>& attachmentsMap);
    void createRenderPass();
    void createFramebuffers();
    void createPipelines();
    void createDescriptorPool();
    void createDescriptorSets();
public:
    customFilter(bool enable, float blitFactor, float xSamplerStep, float ySamplerStep, uint32_t blitAttachmentsCount);

    void destroy() override;
    void create(std::unordered_map<std::string, std::pair<bool,std::vector<attachments*>>>& attachmentsMap) override;
    void updateDescriptorSets(
        const std::unordered_map<std::string, std::pair<VkDeviceSize,std::vector<VkBuffer>>>& bufferMap,
        const std::unordered_map<std::string, std::pair<bool,std::vector<attachments*>>>& attachmentsMap) override;
    void updateCommandBuffer(uint32_t frameNumber) override;

    customFilter& setBlitFactor(const float& blitFactor);
    customFilter& setSamplerStepX(const float& xSamplerStep);
    customFilter& setSamplerStepY(const float& ySamplerStep);
};


#endif // CUSTOMFILTER_H
