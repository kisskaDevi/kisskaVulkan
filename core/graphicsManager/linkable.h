#ifndef LINKABLE_H
#define LINKABLE_H

#include <vulkan.h>

class linkable{
public:
    virtual void draw(VkCommandBuffer commandBuffer, uint32_t imageNumber) const = 0;
    virtual void setRenderPass(VkRenderPass renderPass) = 0;
};

#endif // LINKABLE_H