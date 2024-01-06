#ifndef GRAPHICSLINKER_H
#define GRAPHICSLINKER_H

#include <vector>
#include <stdint.h>

#include <vulkan.h>

class linkable;
class swapChain;

class graphicsLinker
{
private:
    uint32_t                        imageCount;
    VkExtent2D                      imageExtent;
    VkFormat                        imageFormat;
    VkDevice                        device;

    std::vector<linkable*>          linkables;
    swapChain*                      swapChainKHR{nullptr};

    VkRenderPass                    renderPass{VK_NULL_HANDLE};
    std::vector<VkFramebuffer>      framebuffers;

    VkCommandPool                   commandPool;
    std::vector<VkCommandBuffer>    commandBuffers;
    std::vector<bool>               updateCommandBufferFlags;

    std::vector<VkSemaphore>        signalSemaphores;
public:
    graphicsLinker() = default;
    ~graphicsLinker();
    void destroy();

    void setSwapChain(swapChain* swapChainKHR);
    void setDevice(VkDevice device);
    void addLinkable(linkable* link);

    void createRenderPass();
    void createFramebuffers();

    void createCommandPool();
    void createCommandBuffers();
    void updateCommandBuffer(uint32_t resourceNumber, uint32_t imageNumber);

    void createSyncObjects();
    const VkSemaphore& submit(uint32_t frameNumber, const std::vector<VkSemaphore>& waitSemaphores, VkFence fence, VkQueue queue);

    const VkRenderPass& getRenderPass() const;
    const VkCommandBuffer& getCommandBuffer(uint32_t frameNumber) const;

    void updateCmdFlags();
};

#endif // GRAPHICSLINKER_H
