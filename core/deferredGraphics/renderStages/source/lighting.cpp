#include "graphics.h"
#include "vkdefault.h"
#include "light.h"
#include "deferredAttachments.h"
#include "depthMap.h"
#include "operations.h"

namespace moon::deferredGraphics {

void Graphics::Lighting::destroy(VkDevice device) {
    if(DescriptorPool) {vkDestroyDescriptorPool(device, DescriptorPool, nullptr); DescriptorPool = VK_NULL_HANDLE;}
}

void Graphics::Lighting::createDescriptorSetLayout(VkDevice device)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back(moon::utils::vkDefault::inAttachmentFragmentLayoutBinding(static_cast<uint32_t>(bindings.size()), 1));
        bindings.push_back(moon::utils::vkDefault::inAttachmentFragmentLayoutBinding(static_cast<uint32_t>(bindings.size()), 1));
        bindings.push_back(moon::utils::vkDefault::inAttachmentFragmentLayoutBinding(static_cast<uint32_t>(bindings.size()), 1));
        bindings.push_back(moon::utils::vkDefault::inAttachmentFragmentLayoutBinding(static_cast<uint32_t>(bindings.size()), 1));
        bindings.push_back(moon::utils::vkDefault::bufferVertexLayoutBinding(static_cast<uint32_t>(bindings.size()), 1));

    CHECK(lightingDescriptorSetLayout.create(device, bindings));

    bufferDescriptorSetLayoutMap[moon::interfaces::LightType::spot] = moon::interfaces::Light::createBufferDescriptorSetLayout(device);
    textureDescriptorSetLayoutMap[moon::interfaces::LightType::spot] = moon::interfaces::Light::createTextureDescriptorSetLayout(device);
    shadowDescriptorSetLayout = moon::utils::DepthMap::createDescriptorSetLayout(device);
}

void Graphics::Lighting::createPipeline(VkDevice device, moon::utils::ImageInfo* pInfo, VkRenderPass pRenderPass)
{
    std::filesystem::path spotVert = shadersPath / "spotLightingPass/spotLightingVert.spv";
    std::filesystem::path spotFrag = shadersPath / "spotLightingPass/spotLightingFrag.spv";
    createPipeline(moon::interfaces::LightType::spot, device, pInfo, pRenderPass, spotVert, spotFrag);
}

void Graphics::createLightingDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.push_back(VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, static_cast<uint32_t>(5 * image.Count)});
    poolSizes.push_back(VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(image.Count)});

    VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(image.Count);
    CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &lighting.DescriptorPool));
}

void Graphics::createLightingDescriptorSets()
{
    lighting.DescriptorSets.resize(image.Count);
    std::vector<VkDescriptorSetLayout> layouts(image.Count, lighting.lightingDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = lighting.DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(image.Count);
        allocInfo.pSetLayouts = layouts.data();
    CHECK(vkAllocateDescriptorSets(device, &allocInfo, lighting.DescriptorSets.data()));
}

void Graphics::updateLightingDescriptorSets(const moon::utils::BuffersDatabase& bDatabase)
{
    for (uint32_t i = 0; i < image.Count; i++){
        std::vector<VkDescriptorImageInfo> imageInfos;
        imageInfos.push_back(VkDescriptorImageInfo{
            VK_NULL_HANDLE,
            deferredAttachments.GBuffer.position.imageView(i),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });
        imageInfos.push_back(VkDescriptorImageInfo{
            VK_NULL_HANDLE,
            deferredAttachments.GBuffer.normal.imageView(i),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });
        imageInfos.push_back(VkDescriptorImageInfo{
            VK_NULL_HANDLE,
            deferredAttachments.GBuffer.color.imageView(i),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });
        imageInfos.push_back(VkDescriptorImageInfo{
            VK_NULL_HANDLE,
            deferredAttachments.GBuffer.depth.imageView(i),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        });

        VkDescriptorBufferInfo bufferInfo = bDatabase.descriptorBufferInfo(parameters.in.camera, i);

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        for(auto& imageInfo: imageInfos){
            descriptorWrites.push_back(VkWriteDescriptorSet{});
                descriptorWrites.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites.back().dstSet = lighting.DescriptorSets[i];
                descriptorWrites.back().dstBinding = static_cast<uint32_t>(descriptorWrites.size() - 1);
                descriptorWrites.back().dstArrayElement = 0;
                descriptorWrites.back().descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                descriptorWrites.back().descriptorCount = 1;
                descriptorWrites.back().pImageInfo = &imageInfo;
        }
        descriptorWrites.push_back(VkWriteDescriptorSet{});
            descriptorWrites.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites.back().dstSet = lighting.DescriptorSets[i];
            descriptorWrites.back().dstBinding = static_cast<uint32_t>(descriptorWrites.size() - 1);
            descriptorWrites.back().dstArrayElement = 0;
            descriptorWrites.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites.back().descriptorCount = 1;
            descriptorWrites.back().pBufferInfo = &bufferInfo;
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void Graphics::Lighting::render(uint32_t frameNumber, VkCommandBuffer commandBuffer) const
{
    for(auto& lightSource: *lightSources){
        uint8_t mask = lightSource->getPipelineBitMask();
        lightSource->render(frameNumber, commandBuffer, {DescriptorSets[frameNumber], (*depthMaps)[lightSource]->getDescriptorSets()[frameNumber]}, pipelineLayoutMap.at(mask), pipelineMap.at(mask));
    }
}

}
