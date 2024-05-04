#ifndef CAMERA_H
#define CAMERA_H

#include <vulkan.h>
#include "buffer.h"

namespace moon::utils { struct PhysicalDevice;}

class camera{
public:
    virtual ~camera(){};

    virtual void destroy(VkDevice device) = 0;

    virtual const moon::utils::Buffers& getBuffers() const = 0;

    virtual void create(moon::utils::PhysicalDevice device, uint32_t imageCount) = 0;
    virtual void update(uint32_t frameNumber, VkCommandBuffer commandBuffer) = 0;
};

#endif // CAMERA_H
