#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include "workflow.h"

namespace moon::workflows {

struct PostProcessingParameters{
    struct{
        std::string baseColor;
        std::string blur;
        std::string bloom;
        std::string ssao;
        std::string boundingBox;
    }in;
    struct{
        std::string postProcessing;
    }out;
};

class PostProcessingGraphics : public Workflow
{
private:
    PostProcessingParameters parameters;

    moon::utils::Attachments frame;
    bool enable{true};

    struct PostProcessing : public Workbody{
        void createPipeline(VkDevice device, moon::utils::ImageInfo* pInfo, VkRenderPass pRenderPass) override;
        void createDescriptorSetLayout(VkDevice device) override;
    }postProcessing;

    void createAttachments(moon::utils::AttachmentsDatabase& aDatabase);
    void createRenderPass();
    void createFramebuffers();
    void createPipelines();
    void createDescriptorPool();
    void createDescriptorSets();
public:
    PostProcessingGraphics(PostProcessingParameters parameters, bool enable);

    void destroy() override;
    void create(moon::utils::AttachmentsDatabase& aDatabase) override;
    void updateDescriptorSets(const moon::utils::BuffersDatabase& bDatabase, const moon::utils::AttachmentsDatabase& aDatabase) override;
    void updateCommandBuffer(uint32_t frameNumber) override;
};

}
#endif // POSTPROCESSING_H
