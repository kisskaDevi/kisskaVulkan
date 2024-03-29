#include "link.h"
#include "operations.h"
#include "vkdefault.h"

void link::destroy(){
    if(Pipeline)            {vkDestroyPipeline(device, Pipeline, nullptr); Pipeline = VK_NULL_HANDLE;}
    if(PipelineLayout)      {vkDestroyPipelineLayout(device, PipelineLayout,nullptr); PipelineLayout = VK_NULL_HANDLE;}
    if(DescriptorSetLayout) {vkDestroyDescriptorSetLayout(device, DescriptorSetLayout, nullptr); DescriptorSetLayout = VK_NULL_HANDLE;}
    if(DescriptorPool)      {vkDestroyDescriptorPool(device, DescriptorPool, nullptr); DescriptorPool = VK_NULL_HANDLE;}
    DescriptorSets.clear();
}

void link::setDeviceProp(VkDevice device){
    this->device = device;
}

void link::setImageCount(const uint32_t& count){
    this->imageCount = count;
}

void link::setShadersPath(const std::filesystem::path &shadersPath){
    this->shadersPath = shadersPath;
}

void link::setRenderPass(VkRenderPass renderPass)
{
    this->renderPass = renderPass;
}

void link::setPositionInWindow(const vector<float,2>& offset, const vector<float,2>& size){
    pushConstant.offset = offset;
    pushConstant.size = size;
}

void link::createDescriptorSetLayout() {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.push_back(vkDefault::imageFragmentLayoutBinding(static_cast<uint32_t>(bindings.size()), 1));
    VkDescriptorSetLayoutCreateInfo textureLayoutInfo{};
        textureLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        textureLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        textureLayoutInfo.pBindings = bindings.data();
    CHECK(vkCreateDescriptorSetLayout(device, &textureLayoutInfo, nullptr, &DescriptorSetLayout));
}

void link::createPipeline(imageInfo* pInfo) {

    auto vertShaderCode = ShaderModule::readFile(shadersPath / "linkable/deferredLinkableVert.spv");
    auto fragShaderCode = ShaderModule::readFile(shadersPath / "linkable/deferredLinkableFrag.spv");
    VkShaderModule vertShaderModule = ShaderModule::create(&device, vertShaderCode);
    VkShaderModule fragShaderModule = ShaderModule::create(&device, fragShaderCode);
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {vkDefault::vertrxShaderStage(vertShaderModule), vkDefault::fragmentShaderStage(fragShaderModule)};

    VkViewport viewport = vkDefault::viewport({0,0}, pInfo->Extent);
    VkRect2D scissor = vkDefault::scissor({0,0}, pInfo->Extent);
    VkPipelineViewportStateCreateInfo viewportState = vkDefault::viewportState(&viewport, &scissor);
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkDefault::vertexInputState();
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkDefault::inputAssembly();
    VkPipelineRasterizationStateCreateInfo rasterizer = vkDefault::rasterizationState();
    VkPipelineMultisampleStateCreateInfo multisampling = vkDefault::multisampleState();
    VkPipelineDepthStencilStateCreateInfo depthStencil = vkDefault::depthStencilDisable();

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment = {vkDefault::colorBlendAttachmentState(VK_FALSE)};
    VkPipelineColorBlendStateCreateInfo colorBlending = vkDefault::colorBlendState(static_cast<uint32_t>(colorBlendAttachment.size()),colorBlendAttachment.data());

    std::vector<VkPushConstantRange> pushConstantRange;
        pushConstantRange.push_back(VkPushConstantRange{});
        pushConstantRange.back().stageFlags = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
        pushConstantRange.back().offset = 0;
        pushConstantRange.back().size = sizeof(linkPushConstant);
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRange.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();
    CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &PipelineLayout));

    std::vector<VkGraphicsPipelineCreateInfo> pipelineInfo;
    pipelineInfo.push_back(VkGraphicsPipelineCreateInfo{});
        pipelineInfo.back().sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.back().pNext = nullptr;
        pipelineInfo.back().stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.back().pStages = shaderStages.data();
        pipelineInfo.back().pVertexInputState = &vertexInputInfo;
        pipelineInfo.back().pInputAssemblyState = &inputAssembly;
        pipelineInfo.back().pViewportState = &viewportState;
        pipelineInfo.back().pRasterizationState = &rasterizer;
        pipelineInfo.back().pMultisampleState = &multisampling;
        pipelineInfo.back().pColorBlendState = &colorBlending;
        pipelineInfo.back().layout = PipelineLayout;
        pipelineInfo.back().renderPass = renderPass;
        pipelineInfo.back().subpass = 0;
        pipelineInfo.back().basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.back().pDepthStencilState = &depthStencil;
    CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, static_cast<uint32_t>(pipelineInfo.size()), pipelineInfo.data(), nullptr, &Pipeline));

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void link::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.push_back(VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageCount});
    VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = imageCount;
    CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &DescriptorPool));
}

void link::createDescriptorSets() {
    DescriptorSets.resize(imageCount);
    std::vector<VkDescriptorSetLayout> layouts(imageCount, DescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(imageCount);
        allocInfo.pSetLayouts = layouts.data();
    CHECK(vkAllocateDescriptorSets(device, &allocInfo, DescriptorSets.data()));
}

void link::updateDescriptorSets(attachments* attachment) {
    for (size_t image = 0; image < this->imageCount; image++)
    {
        VkDescriptorImageInfo imageInfo;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = attachment->instances[image].imageView;
            imageInfo.sampler = attachment->sampler;

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        descriptorWrites.push_back(VkWriteDescriptorSet{});
            descriptorWrites.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites.back().dstSet = DescriptorSets[image];
            descriptorWrites.back().dstBinding = static_cast<uint32_t>(descriptorWrites.size() - 1);
            descriptorWrites.back().dstArrayElement = 0;
            descriptorWrites.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites.back().descriptorCount = 1;
            descriptorWrites.back().pImageInfo = &imageInfo;
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void link::draw(VkCommandBuffer commandBuffer, uint32_t imageNumber) const
{
    vkCmdPushConstants(commandBuffer, PipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof(linkPushConstant), &pushConstant);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSets[imageNumber], 0, nullptr);
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}
