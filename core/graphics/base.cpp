#include "graphics.h"
#include "core/operations.h"
#include "core/transformational/object.h"
#include "core/transformational/gltfmodel.h"

#include <array>

void graphics::Base::Destroy(VkApplication *app)
{
    vkDestroyPipeline(app->getDevice(), Pipeline, nullptr);
    vkDestroyPipelineLayout(app->getDevice(), PipelineLayout,nullptr);
    vkDestroyDescriptorSetLayout(app->getDevice(), SceneDescriptorSetLayout,  nullptr);
    vkDestroyDescriptorSetLayout(app->getDevice(), ObjectDescriptorSetLayout,  nullptr);
    vkDestroyDescriptorSetLayout(app->getDevice(), PrimitiveDescriptorSetLayout,  nullptr);
    vkDestroyDescriptorSetLayout(app->getDevice(), MaterialDescriptorSetLayout,  nullptr);
    vkDestroyDescriptorPool(app->getDevice(), DescriptorPool, nullptr);

    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        vkDestroyBuffer(app->getDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(app->getDevice(), uniformBuffersMemory[i], nullptr);
    }
}

void graphics::Base::createDescriptorSetLayout(VkApplication *app)
{
    uint32_t index = 0;

    std::array<VkDescriptorSetLayoutBinding, 1> uboLayoutBinding{};
        uboLayoutBinding[index].binding = 0;
        uboLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding[index].descriptorCount = 1;
        uboLayoutBinding[index].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding[index].pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(uboLayoutBinding.size());
        layoutInfo.pBindings = uboLayoutBinding.data();
    if (vkCreateDescriptorSetLayout(app->getDevice(), &layoutInfo, nullptr, &SceneDescriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create base uniform buffer descriptor set layout!");

    index = 0;
    std::array<VkDescriptorSetLayoutBinding, 1> uniformBufferLayoutBinding{};
        uniformBufferLayoutBinding[index].binding = 0;
        uniformBufferLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferLayoutBinding[index].descriptorCount = 1;
        uniformBufferLayoutBinding[index].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uniformBufferLayoutBinding[index].pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo uniformBufferLayoutInfo{};
        uniformBufferLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uniformBufferLayoutInfo.bindingCount = static_cast<uint32_t>(uniformBufferLayoutBinding.size());
        uniformBufferLayoutInfo.pBindings = uniformBufferLayoutBinding.data();
    if (vkCreateDescriptorSetLayout(app->getDevice(), &uniformBufferLayoutInfo, nullptr, &ObjectDescriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create base object uniform buffer descriptor set layout!");

    index = 0;
    std::array<VkDescriptorSetLayoutBinding, 1> uniformBlockLayoutBinding{};
        uniformBlockLayoutBinding[index].binding = 0;
        uniformBlockLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBlockLayoutBinding[index].descriptorCount = 1;
        uniformBlockLayoutBinding[index].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uniformBlockLayoutBinding[index].pImmutableSamplers = nullptr;
    VkDescriptorSetLayoutCreateInfo uniformBlockLayoutInfo{};
        uniformBlockLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uniformBlockLayoutInfo.bindingCount = static_cast<uint32_t>(uniformBlockLayoutBinding.size());
        uniformBlockLayoutInfo.pBindings = uniformBlockLayoutBinding.data();
    if (vkCreateDescriptorSetLayout(app->getDevice(), &uniformBlockLayoutInfo, nullptr, &PrimitiveDescriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create base uniform block descriptor set layout!");

    index = 0;
    std::array<VkDescriptorSetLayoutBinding, 5> materialLayoutBinding{};
    //baseColorTexture;
        materialLayoutBinding[index].binding = 0;
        materialLayoutBinding[index].descriptorCount = 1;
        materialLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialLayoutBinding[index].pImmutableSamplers = nullptr;
        materialLayoutBinding[index].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    index++;
    //metallicRoughnessTexture;
        materialLayoutBinding[index].binding = 1;
        materialLayoutBinding[index].descriptorCount = 1;
        materialLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialLayoutBinding[index].pImmutableSamplers = nullptr;
        materialLayoutBinding[index].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    index++;
    //normalTexture;
        materialLayoutBinding[index].binding = 2;
        materialLayoutBinding[index].descriptorCount = 1;
        materialLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialLayoutBinding[index].pImmutableSamplers = nullptr;
        materialLayoutBinding[index].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    index++;
    //occlusionTexture;
        materialLayoutBinding[index].binding = 3;
        materialLayoutBinding[index].descriptorCount = 1;
        materialLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialLayoutBinding[index].pImmutableSamplers = nullptr;
        materialLayoutBinding[index].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    index++;
    //emissiveTexture;
        materialLayoutBinding[index].binding = 4;
        materialLayoutBinding[index].descriptorCount = 1;
        materialLayoutBinding[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialLayoutBinding[index].pImmutableSamplers = nullptr;
        materialLayoutBinding[index].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo materialLayoutInfo{};
        materialLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        materialLayoutInfo.bindingCount = static_cast<uint32_t>(materialLayoutBinding.size());
        materialLayoutInfo.pBindings = materialLayoutBinding.data();
    if (vkCreateDescriptorSetLayout(app->getDevice(), &materialLayoutInfo, nullptr, &MaterialDescriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create base material descriptor set layout!");
}

void graphics::Base::createPipeline(VkApplication *app, graphicsInfo info)
{
    uint32_t index = 0;

    auto vertShaderCode = readFile(ExternalPath + "core\\graphics\\shaders\\base\\basevert.spv");   //?????????????????? ??????????????
    auto fragShaderCode = readFile(ExternalPath + "core\\graphics\\shaders\\base\\basefrag.spv");
    VkShaderModule vertShaderModule = createShaderModule(app, vertShaderCode);                      //?????????????? ?????????????????? ????????????
    VkShaderModule fragShaderModule = createShaderModule(app, fragShaderCode);
    std::array<VkPipelineShaderStageCreateInfo,2> shaderStages{};                           //???????????? ???????????? ???????????????? ?? ??????????????????
        shaderStages[index].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;    //??????????????????
        shaderStages[index].stage = VK_SHADER_STAGE_VERTEX_BIT;                             //???????????????????? ?? ???????? ?????????? ???????????? ???? ???????????????? 222
        shaderStages[index].module = vertShaderModule;                                      //???????? ???????????????? ?????????????????? ????????????
        shaderStages[index].pName = "main";                                                 //?????????????????? ???? ???????????? UTF-8 ?? ?????????????????????? ??????????, ???????????????????????? ?????? ?????????? ?????????? ?????????????? ?????? ?????????? ??????????
    index++;
        shaderStages[index].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;    //??????????????????????
        shaderStages[index].stage = VK_SHADER_STAGE_FRAGMENT_BIT;                           //???????????????????? ?? ???????? ?????????? ???????????? ???? ???????????????? 222
        shaderStages[index].module = fragShaderModule;                                      //???????? ???????????????? ?????????????????? ????????????
        shaderStages[index].pName = "main";                                                 //?????????????????? ???? ???????????? UTF-8 ?? ?????????????????????? ??????????, ???????????????????????? ?????? ?????????? ?????????? ?????????????? ?????? ?????????? ??????????

    /* ?????? ???????????????????? ?????????????????? ?????????????????? ?????? ???????????????????? ???????????????????? ???????????? ?? ???????????????? Vulkan.
     * ???? ???????????? ???????????????????????? ?????????????? ???????????? ?? ??????????????????????, ?????????????????? ?? SPIR-V, ?????? ????????????????????????????
     * ?????????????????? ?????????????????? ?????? ???? ???????? ?????????????????? ???????????????????????????? ???????????? ???? ????????????. ???????????? ?????????? ???? ????????????
     * ?????????????? ???????????????????? ???????????????????????????? ???????????? ?? ????????????, ?? Vulkan ?????????? ?????? ?????????????????? ?????? ???????????? ?????? ??????, ?????????????????? ???? ?????????? ?? ????????????*/

    auto bindingDescription = gltfModel::Vertex::getBindingDescription();
    auto attributeDescriptions = gltfModel::Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    /* ???????? ?????????????? ???????????? ???????????????????????? ?????????????????? ?????????? ???????????? ?? ???????????????? ?? ???????????????????? ???? ?? ??????????????????,
     * ?????????????? ?????? ?????????????????? ???????????????????? ???????????????? ??????????????????.*/

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

    /* ?????????? ?????????? ???????? ?????????????????? ????????????????????*/

    /* ???????????????????????????? ?????????????? ???????????? - ?????? ?????????????????? ???????????????????????????? ?????????????????? ?? ?????????????????? Vulkan ???? ??????????????????????????.
     * ?????? ?????????????????????? ???????????????????? ?????????????? ???? ?????????????????????????????? ?????????????????? ???????????????????? ?? ?????????????? ????????????????????. ????????????????????????
     * ?????????? ???????????????????????????? ?????????????????? ???????????????? ????????????.*/

    index = 0;
    std::array<VkViewport,1> viewport{};
        viewport[index].x = 0.0f;
        viewport[index].y = 0.0f;
        viewport[index].width = (float) info.extent.width;
        viewport[index].height = (float) info.extent.height;
        viewport[index].minDepth = 0.0f;
        viewport[index].maxDepth = 1.0f;
    std::array<VkRect2D,1> scissor{};
        scissor[index].offset = {0, 0};
        scissor[index].extent = info.extent;
    VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = static_cast<uint32_t>(viewport.size());;              //?????????? ???????????????? ????????????
        viewportState.pViewports = viewport.data();                                         //???????????? ???????????? ?????????????? ????????????
        viewportState.scissorCount = static_cast<uint32_t>(scissor.size());;                //?????????? ??????????????????????????????
        viewportState.pScissors = scissor.data();                                           //??????????????

    /* ???????????????????????? - ?????? ??????????????, ?? ???????? ???????????????? ??????????????????, ???????????????????????????? ??????????????????, ?????????????????????????? ?? ???????????? ????????????????????, ?????????????? ?? ??????????????????
     * ?????????????????????? ????????????????. ?????????????????? ???????????????????????? ?????????????????????? ??????, ?????? ???????? ?????????????? ????????????????????, ?? ???????????????? ?????? ???????????? ?????????????????? ??????????????????*/

    VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;                                      //???????????????????????? ?????? ???????? ?????????? ?????????????????? ?????????????????? ????????????????????????. ?????????? ???????? ????????????????????, ???????????????????????? ???? ???????????????? ?? ???? ?????????????????? ??????????????????
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;                                      //???????????????????????? ?????? ???????? ?????????? Vulkan ?????????????????????????? ?????????????????? ???????????????????????? ?? ?????????? ?????? ??????????????
        rasterizer.lineWidth = 1.0f;                                                        //?????????????? ??????????
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;                                        //???????????????? ??????????????????????
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;                             //???????????????? ?????????????????????? ???????????? (???????????? ?????????????? ??????????????)
        rasterizer.depthBiasEnable = VK_FALSE;                                              //???????????????????????? ?????? ???????? ?????????? ???????????????? ?????????????????? ??????????????
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

    /* ?????????????????????????? - ?????? ?????????????? ???????????????? ???????????????????? ???????????????? (sample) ?????? ?????????????? ?????????????? ?? ??????????????????????.
     * ?????? ???????????????????????? ?????? ???????????? ?? ?????????????????????? ?? ?????????? ?????????????? ???????????????? ?????????? ???????????????? ?????????????????????? ?????? ?????????????????????? ??????????????????????????*/

    VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = info.msaaSamples;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

    /* ?????????????????? ?????????????? ?? ?????????????????????? ?????????????????? ???????????????? ???????????? ???????????????????? ????????????. ?????? ???????????? ???????????????? ???? ???????????? ????????????????????
     * ?? ???????????????? ??????????????????????. ???? ???????????? ?????????????? ?????? ?????????????? ????????????????, ?????????????? ???????????? ???????????????????? ???????????????????? ?????????????????? ????????????????
     * ???????????????????????? ?????????????? ???????????? ?????????????? ????????????????. ?????????????????????????????????????? ???????????????????? ???????? ???????????????? ???? ????????????????????,
     * ?????? ???????????????????????? ???? ?????????? ????????????, ?? ???????????????????? ?????????????? ???????????????????? ???????????????? ?????????? ?????????????????? ???????????????????? ????????????????????????
     * ?????????????? ?? ?????????????? ?????????????????????? ??????????????????????.*/

    std::array<VkPipelineColorBlendAttachmentState,6> colorBlendAttachment;
    for(index=0;index<colorBlendAttachment.size();index++)
    {
        colorBlendAttachment[index].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment[index].blendEnable = VK_FALSE;
        colorBlendAttachment[index].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment[index].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment[index].colorBlendOp = VK_BLEND_OP_MAX;
        colorBlendAttachment[index].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment[index].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment[index].alphaBlendOp = VK_BLEND_OP_MAX;
    }
    VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;                                                 //????????????, ???????????????????? ???? ?????????????????? ???????????????????? ???????????????? ?????????? ?????????????? ???????????????????????? ?????????????? ?? ?????????????????????? ???????????????? ??????????????????????
        colorBlending.logicOp = VK_LOGIC_OP_COPY;                                               //Optional
        colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachment.size());     //???????????????????? ??????????????????????
        colorBlending.pAttachments = colorBlendAttachment.data();                               //???????????? ??????????????????????
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

    /* ?????? ???????? ?????????? ?????????????? ?????????????????? ?????????????????? ?????????????????? ?????????? ????????????????, Vulkan ?????????????????????????? ?????????????????????? ????????????????
     * ???????????????????????? ?????????? ???????????????????????? ?????????????????? ?????? ??????????????????????, ?????? ???????????? ?????? ?????? ?????????? ???????? ???????????????? ?????????? ???? ??????????
     * ?????? ???????????? ???????????? ?????????? ???????????? ???????????????????? ????????????*/

    index=0;
    std::array<VkPushConstantRange,1> pushConstantRange{};
        pushConstantRange[index].stageFlags = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
        pushConstantRange[index].offset = 0;
        pushConstantRange[index].size = sizeof(PushConst);
    std::array<VkDescriptorSetLayout,4> setLayouts = {SceneDescriptorSetLayout,ObjectDescriptorSetLayout,PrimitiveDescriptorSetLayout,MaterialDescriptorSetLayout};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRange.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data();
    if (vkCreatePipelineLayout(app->getDevice(), &pipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create base pipeline layout!");

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

    index=0;
    std::array<VkGraphicsPipelineCreateInfo,1> pipelineInfo{};
        pipelineInfo[index].sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo[index].stageCount = static_cast<uint32_t>(shaderStages.size());   //?????????? ???????????????? ?? ?????????????? ????????????????
        pipelineInfo[index].pStages = shaderStages.data();                             //?????????????????? ???? ???????????? ???????????????? VkPipelineShaderStageCreateInfo, ???????????? ???? ?????????????? ???????????????? ???????? ????????????
        pipelineInfo[index].pVertexInputState = &vertexInputInfo;                      //?????????????????? ????????
        pipelineInfo[index].pInputAssemblyState = &inputAssembly;                      //???????? ?????????????? ????????????
        pipelineInfo[index].pViewportState = &viewportState;                           //???????????????????????????? ?????????????? ????????????
        pipelineInfo[index].pRasterizationState = &rasterizer;                         //????????????????????????
        pipelineInfo[index].pMultisampleState = &multisampling;                        //??????????????????????????
        pipelineInfo[index].pColorBlendState = &colorBlending;                         //???????????????????? ????????????
        pipelineInfo[index].layout = PipelineLayout;                                   //
        pipelineInfo[index].renderPass = info.renderPass;                              //???????????? ????????????????????
        pipelineInfo[index].subpass = 0;                                               //?????????????????? ????????????????????
        pipelineInfo[index].pDepthStencilState = &depthStencil;
        pipelineInfo[index].basePipelineHandle = VK_NULL_HANDLE;
    if (vkCreateGraphicsPipelines(app->getDevice(), VK_NULL_HANDLE, static_cast<uint32_t>(pipelineInfo.size()), pipelineInfo.data(), nullptr, &Pipeline) != VK_SUCCESS)
        throw std::runtime_error("failed to create base graphics pipeline!");

    //?????????? ?????????????? ?????????????????? ???????????? ?????????? ??????????????????????????
    vkDestroyShaderModule(app->getDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(app->getDevice(), vertShaderModule, nullptr);
}

void graphics::createBaseDescriptorPool()
{
    /* ???????????? ???????????????????????? ???????????? ?????????????????? ????????????????, ?????? ???????????? ???????????????????? ???? ????????, ?????? ???????????? ????????????.
     * ???????????????????? ?????? ?????????????? ???????????????????????? ?????????????????????????? ???????????????????? ?????????? ???????????????????????? . ???? ??????????????
     * ?????????? ?????????????? createDescriptorPool ?????? ???? ??????????????????.*/

    uint32_t index = 0;

    std::array<VkDescriptorPoolSize,1> poolSizes;
        poolSizes.at(index).type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;                           //?????????????? ?????? ?????????? ??????????????, ?????????? ???????? ???????????????????????? ?????????? ?????????????????? ???????? ???????????? ????????????????????????
        poolSizes.at(index).descriptorCount = static_cast<uint32_t>(image.Count);                //?? ?????????????? ????, ?????????????????? VkDescriptorPoolSize??????????????????.
    VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(image.Count);
    if (vkCreateDescriptorPool(app->getDevice(), &poolInfo, nullptr, &base.DescriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create base descriptor pool!");
}

void graphics::createBaseDescriptorSets()
{
    //???????????? ???? ?????????? ???????????????? ???????? ???????????? ????????????????????????
    /* ?? ?????????? ???????????? ???? ???????????????? ???????? ?????????? ???????????????????????? ?????? ?????????????? ?????????????????????? ?????????????? ????????????????, ?????? ?? ???????????????????? ??????????????.
     * ?? ??????????????????, ?????? ?????????? ?????? ?????????? ????????????, ???????????? ?????? ?????????????????? ?????????????? ?????????????? ????????????, ?????????????????????????????? ???????????????????? ??????????????.
     * ???????????????? ???????? ???????????? ?????? ???????????????? ???????????????????????? ???????????? ???????????????????????? ?? ?????????????????? ???? vkAllocateDescriptorSets */

    base.DescriptorSets.resize(image.Count);
    std::vector<VkDescriptorSetLayout> layouts(image.Count, base.SceneDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = base.DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(image.Count);
        allocInfo.pSetLayouts = layouts.data();
    if (vkAllocateDescriptorSets(app->getDevice(), &allocInfo, base.DescriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate base descriptor sets!");

    //???????????? ???????????????????????? ?????? ????????????????, ???? ?????????????????????? ???????????? ?????? ?????????????????? ?? ??????????????????.
    //???????????? ???? ?????????????? ???????? ?????? ???????????????????? ?????????????? ??????????????????????:
    for (size_t i = 0; i < image.Count; i++)
    {
        uint32_t index = 0;

        VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = base.uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);
        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
            descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[index].dstSet = base.DescriptorSets[i];
            descriptorWrites[index].dstBinding = 0;
            descriptorWrites[index].dstArrayElement = 0;
            descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[index].descriptorCount = 1;
            descriptorWrites[index].pBufferInfo = &bufferInfo;
        vkUpdateDescriptorSets(app->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void graphics::Base::createUniformBuffers(VkApplication *app, uint32_t imageCount)
{
    uniformBuffers.resize(imageCount);
    uniformBuffersMemory.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        createBuffer(app, sizeof(UniformBufferObject),
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     uniformBuffers[i], uniformBuffersMemory[i]);
    }
}

void graphics::Base::render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i, graphics *Graphics)
{
    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
    for(size_t j = 0; j<objects.size() ;j++)
    {
        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, & objects[j]->getModel()->vertices.buffer, offsets);
        if (objects[j]->getModel()->indices.buffer != VK_NULL_HANDLE)
            vkCmdBindIndexBuffer(commandBuffers[i], objects[j]->getModel()->indices.buffer, 0, VK_INDEX_TYPE_UINT32);

        for (auto node : objects[j]->getModel()->nodes)
            Graphics->renderNode(node,commandBuffers[i],DescriptorSets[i],objects[j]->getDescriptorSet()[i],PipelineLayout);

//            for (auto node : object3D[j]->getModel()->nodes)
//               if(glm::length(glm::vec3(object3D[j]->getTransformation()*node->matrix*glm::vec4(0.0f,0.0f,0.0f,1.0f))-cameraPosition)<object3D[j]->getVisibilityDistance())
//                    renderNode(node,commandBuffers[i],base.DescriptorSets[i],object3D[j]->getDescriptorSet()[i],*object3D[j]->getPipelineLayout());
    }
}

void graphics::Base::setMaterials(std::vector<PushConstBlockMaterial> &nodeMaterials, graphics *Graphics)
{
    for(size_t j = 0; j<objects.size() ;j++)
        for (auto node : objects[j]->getModel()->nodes){
            uint32_t objectPrimitive = 0;
            Graphics->setMaterialNode(node,nodeMaterials,objectPrimitive,objects[j]->getModel()->firstPrimitive);
        }
}
