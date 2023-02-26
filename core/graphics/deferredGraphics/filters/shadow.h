#ifndef SHADOW_H
#define SHADOW_H

#include "filtergraphics.h"
#include <unordered_map>

class object;
class Node;
class light;

class shadowGraphics : public filterGraphics
{
private:
    VkPhysicalDevice*                   physicalDevice{nullptr};
    VkDevice*                           device{nullptr};

    texture*                            emptyTexture{nullptr};

    imageInfo                           image;

    std::vector<attachments*>           pAttachments;

    VkRenderPass                        renderPass{VK_NULL_HANDLE};
    std::unordered_map<light*,std::vector<VkFramebuffer>> framebuffers;

    std::vector<VkCommandBuffer>          commandBuffers;

    struct Shadow : public filter{
        std::string                     ExternalPath;

        VkPipelineLayout                PipelineLayout{VK_NULL_HANDLE};
        VkPipeline                      Pipeline{VK_NULL_HANDLE};
        VkDescriptorSetLayout           lightUniformBufferSetLayout{VK_NULL_HANDLE};
        VkDescriptorSetLayout           uniformBufferSetLayout{VK_NULL_HANDLE};
        VkDescriptorSetLayout           uniformBlockSetLayout{VK_NULL_HANDLE};

        std::vector<object*>            objects;
        std::vector<light*>             lightSources;

        void Destroy(VkDevice* device) override;
        void createPipeline(VkDevice* device, imageInfo* pInfo, VkRenderPass* pRenderPass) override;
        void createDescriptorSetLayout(VkDevice* device) override;
    }shadow;

    void render(uint32_t frameNumber, VkCommandBuffer commandBuffer, uint32_t attachmentNumber);
    void renderNode(VkCommandBuffer commandBuffer, Node* node, uint32_t descriptorSetsCount, VkDescriptorSet* descriptorSets);
public:
    shadowGraphics();
    void destroy() override;
    void freeCommandBuffer(VkCommandPool commandPool){
        if(commandBuffers.data()){
            vkFreeCommandBuffers(*device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        }
        commandBuffers.resize(0);
    }

    void setEmptyTexture(texture* emptyTexture) override;
    void setExternalPath(const std::string& path) override;
    void setDeviceProp(VkPhysicalDevice* physicalDevice, VkDevice* device) override;
    void setImageProp(imageInfo* pInfo) override;

    void setAttachments(uint32_t attachmentsCount, attachments* pAttachments) override;
    void createAttachments(uint32_t attachmentsCount, attachments* pAttachments) override;
    void createRenderPass() override;
    void createFramebuffers() override {}
    void createPipelines() override;

    void createDescriptorPool() override {}
    void createDescriptorSets() override {}

    void createCommandBuffers(VkCommandPool commandPool) override;
    void updateCommandBuffer(uint32_t frameNumber) override;
    VkCommandBuffer& getCommandBuffer(uint32_t frameNumber) override;

    void createFramebuffers(light* lightSource);

    void addLightSource(light* lightSource);
    void removeLightSource(light* lightSource);

    void bindBaseObject(object* newObject);
    bool removeBaseObject(object* object);
};

#endif // SHADOW_H