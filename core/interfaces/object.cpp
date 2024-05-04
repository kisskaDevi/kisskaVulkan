#include "object.h"
#include "vkdefault.h"
#include "operations.h"

namespace moon::interfaces {

bool Object::getEnable() const {
    return enable;
}

bool Object::getEnableShadow() const {
    return enableShadow;
}

void Object::setEnable(const bool& enable) {
    this->enable = enable;
}

void Object::setEnableShadow(const bool& enable) {
    this->enableShadow = enable;
}

void Object::setModel(Model* model, uint32_t firstInstance, uint32_t instanceCount){
    this->pModel = model;
    this->firstInstance = firstInstance;
    this->instanceCount = instanceCount;
}

Model* Object::getModel() {
    return pModel;
}

uint32_t Object::getInstanceNumber(uint32_t imageNumber) const {
    return firstInstance + (instanceCount > imageNumber ? imageNumber : 0);
}

void Object::setOutlining(const bool& enable, const float& width, const vector<float,4>& color){
    outlining.Enable = enable;
    outlining.Width = width > 0.0f ? width : outlining.Width;
    outlining.Color = dot(color,color) > 0.0f ? color : outlining.Color;

    pipelineBitMask |= outlining.Enable ? ObjectProperty::outlining : ObjectProperty::non;
}

bool Object::getOutliningEnable() const {
    return outlining.Enable;
}

float Object::getOutliningWidth() const {
    return outlining.Width;
}

vector<float,4> Object::getOutliningColor() const {
    return outlining.Color;
}

void Object::setFirstPrimitive(uint32_t firstPrimitive) {
    this->firstPrimitive = firstPrimitive;
}

void Object::setPrimitiveCount(uint32_t primitiveCount) {
    this->primitiveCount = primitiveCount;
}

bool Object::comparePrimitive(uint32_t primitive) {
    return !(primitive < firstPrimitive) && (primitive < firstPrimitive + primitiveCount);
}

uint32_t Object::getFirstPrimitive() const {
    return firstPrimitive;
}

uint32_t Object::getPrimitiveCount() const {
    return primitiveCount;
}

const std::vector<VkDescriptorSet> &Object::getDescriptorSet() const {
    return descriptors;
}

uint8_t Object::getPipelineBitMask() const {
    return pipelineBitMask;
}

void Object::createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout* descriptorSetLayout){
    std::vector<VkDescriptorSetLayoutBinding> binding;
    binding.push_back(moon::utils::vkDefault::bufferVertexLayoutBinding(static_cast<uint32_t>(binding.size()), 1));
    binding.back().stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo uniformBufferLayoutInfo{};
        uniformBufferLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uniformBufferLayoutInfo.bindingCount = static_cast<uint32_t>(binding.size());
        uniformBufferLayoutInfo.pBindings = binding.data();
    CHECK(vkCreateDescriptorSetLayout(device, &uniformBufferLayoutInfo, nullptr, descriptorSetLayout));
}

void Object::createSkyboxDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout* descriptorSetLayout)
{
    std::vector<VkDescriptorSetLayoutBinding> binding;
    binding.push_back(moon::utils::vkDefault::bufferVertexLayoutBinding(static_cast<uint32_t>(binding.size()), 1));
    binding.push_back(moon::utils::vkDefault::imageFragmentLayoutBinding(static_cast<uint32_t>(binding.size()), 1));
    VkDescriptorSetLayoutCreateInfo uniformBufferLayoutInfo{};
        uniformBufferLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uniformBufferLayoutInfo.bindingCount = static_cast<uint32_t>(binding.size());
        uniformBufferLayoutInfo.pBindings = binding.data();
    CHECK(vkCreateDescriptorSetLayout(device, &uniformBufferLayoutInfo, nullptr, descriptorSetLayout));
}

}
