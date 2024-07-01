#include "workflow.h"
#include "operations.h"

namespace moon::workflows {

Workflow& Workflow::setDeviceProp(VkPhysicalDevice physicalDevice, VkDevice device){
    this->physicalDevice = physicalDevice;
    this->device = device;
    return *this;
}

void Workflow::createCommandBuffers(const utils::vkDefault::CommandPool& commandPool) {
    commandBuffers = commandPool.allocateCommandBuffers(imageInfo.Count);
}

void Workflow::beginCommandBuffer(uint32_t frameNumber) const {
    CHECK(commandBuffers[frameNumber].reset());
    CHECK(commandBuffers[frameNumber].begin());
}

void Workflow::endCommandBuffer(uint32_t frameNumber) const {
    CHECK(commandBuffers[frameNumber].end());
}

VkCommandBuffer Workflow::getCommandBuffer(uint32_t frameNumber) const{
    return commandBuffers[frameNumber];
}

}
