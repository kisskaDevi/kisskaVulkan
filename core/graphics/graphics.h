#ifndef GRAPHICS_H
#define GRAPHICS_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <libs/glfw-3.3.4.bin.WIN64/include/GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <libs/glfw-3.3.4.bin.WIN64/include/GLFW/glfw3native.h>

#include <libs/glm/glm/glm.hpp>
#include <libs/glm/glm/gtc/matrix_transform.hpp>

#include <optional>         // нужна для вызова std::optional<uint32_t>
#include "attachments.h"

class                               VkApplication;
class                               texture;
class                               cubeTexture;
class                               object;
class                               camera;
template <typename type> class      light;
class                               spotLight;
template <> class                   light<spotLight>;
struct                              Node;
struct                              Material;
struct                              PushConstBlockMaterial;

struct graphicsInfo{
    uint32_t                        imageCount;
    VkExtent2D                      extent;
    VkSampleCountFlagBits           msaaSamples;
    VkRenderPass                    renderPass;
};

struct UniformBufferObject{
    alignas(16) glm::mat4           view;
    alignas(16) glm::mat4           proj;
    alignas(16) glm::vec4           eyePosition;
};

struct SkyboxUniformBufferObject{
    alignas(16) glm::mat4           proj;
    alignas(16) glm::mat4           view;
    alignas(16) glm::mat4           model;
};

struct SecondUniformBufferObject{
    alignas(16) glm::vec4           eyePosition;
};

struct SwapChainSupportDetails{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

struct Objects{
    std::vector<object *> *base;
    std::vector<object *> *bloom;
    std::vector<object *> *stencil;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t>         graphicsFamily;                     //графикческое семейство очередей
    std::optional<uint32_t>         presentFamily;                      //семейство очередей показа
    bool isComplete()                                                   //если оба значения не пусты, а были записаны, выводит true
    {return graphicsFamily.has_value() && presentFamily.has_value();}
    //std::optional это оболочка, которая не содержит значения, пока вы ей что-то не присвоите.
    //В любой момент вы можете запросить, содержит ли он значение или нет, вызвав его has_value()функцию-член.
};

struct PushConst
{
    alignas(4)  int                 normalTextureSet;
    alignas(4)  uint32_t            number;
};

struct StencilPushConst
{
    alignas(16) glm::vec4           stencilColor;
};

class graphics
{
private:
    VkApplication                   *app;
    texture                         *emptyTexture;
    glm::vec3                       cameraPosition;
    uint32_t                        primitiveCount = 0;

    struct Image{
        uint32_t                        Count;
        VkFormat                        Format;
        VkExtent2D                      Extent;
        VkSampleCountFlagBits           Samples = VK_SAMPLE_COUNT_1_BIT;
    }image;

    std::vector<attachment>         colorAttachments;
    attachment                      depthAttachment;
    std::vector<attachments>        Attachments;

    VkRenderPass                    renderPass;
    std::vector<VkFramebuffer>      framebuffers;

    struct Base{
        VkPipelineLayout                PipelineLayout;
        VkPipeline                      Pipeline;
        VkDescriptorSetLayout           SceneDescriptorSetLayout;
        VkDescriptorSetLayout           ObjectDescriptorSetLayout;
        VkDescriptorSetLayout           PrimitiveDescriptorSetLayout;
        VkDescriptorSetLayout           MaterialDescriptorSetLayout;
        VkDescriptorPool                DescriptorPool;
        std::vector<VkDescriptorSet>    DescriptorSets;
        std::vector<VkBuffer>           uniformBuffers;
        std::vector<VkDeviceMemory>     uniformBuffersMemory;

        std::vector<object *>           objects;

        void Destroy(VkApplication  *app);
        void createPipeline(VkApplication *app, graphicsInfo info);
        void createDescriptorSetLayout(VkApplication *app);
        void createUniformBuffers(VkApplication *app, uint32_t imageCount);
        void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i, graphics *Graphics);
        void setMaterials(std::vector<PushConstBlockMaterial> &nodeMaterials, graphics *Graphics);
    }base;

    struct bloomExtension{
        VkPipeline                      Pipeline;
        VkPipelineLayout                PipelineLayout;

        std::vector<object *>           objects;

        void Destroy(VkApplication  *app);
        void createPipeline(VkApplication *app, Base *base, graphicsInfo info);
        void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i, graphics *Graphics, Base *base);
        void setMaterials(std::vector<PushConstBlockMaterial> &nodeMaterials, graphics *Graphics);
    }bloom;

    struct godRaysExtension{
        VkPipeline                      Pipeline;
        VkPipelineLayout                PipelineLayout;

        std::vector<object *>           objects;

        void Destroy(VkApplication  *app);
        void createPipeline(VkApplication *app, Base *base, graphicsInfo info);
        void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i, graphics *Graphics);
        void setMaterials(std::vector<PushConstBlockMaterial> &nodeMaterials, graphics *Graphics);
    }godRays;

    struct StencilExtension{
        VkPipeline                      firstPipeline;
        VkPipeline                      secondPipeline;
        VkPipelineLayout                firstPipelineLayout;
        VkPipelineLayout                secondPipelineLayout;

        std::vector<bool>               stencilEnable;
        std::vector<float>              stencilWidth;
        std::vector<glm::vec4>          stencilColor;
        std::vector<object *>           objects;

        void DestroyFirstPipeline(VkApplication *app);
        void DestroySecondPipeline(VkApplication *app);
        void createFirstPipeline(VkApplication *app, Base *base, graphicsInfo info);
        void createSecondPipeline(VkApplication *app, Base *base, graphicsInfo info);
        void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i, graphics *Graphics, Base *base);
        void setMaterials(std::vector<PushConstBlockMaterial> &nodeMaterials, graphics *Graphics);
    }stencil;

    struct Skybox
    {
        cubeTexture                     *texture;
        VkPipelineLayout                PipelineLayout;
        VkPipeline                      Pipeline;
        VkDescriptorSetLayout           DescriptorSetLayout;
        VkDescriptorPool                DescriptorPool;
        std::vector<VkDescriptorSet>    DescriptorSets;
        std::vector<VkBuffer>           uniformBuffers;
        std::vector<VkDeviceMemory>     uniformBuffersMemory;

        std::vector<object *>           objects;

        void Destroy(VkApplication  *app);
        void createPipeline(VkApplication *app, graphicsInfo info);
        void createDescriptorSetLayout(VkApplication *app);
        void createUniformBuffers(VkApplication *app, uint32_t imageCount);
        void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i);
    }skybox;

    struct Second{
        VkPipelineLayout                PipelineLayout;
        VkPipeline                      Pipeline;
        VkDescriptorSetLayout           DescriptorSetLayout;
        VkDescriptorPool                DescriptorPool;
        std::vector<VkDescriptorSet>    DescriptorSets;
        std::vector<VkBuffer>           uniformBuffers;
        std::vector<VkDeviceMemory>     uniformBuffersMemory;
        std::vector<VkBuffer>           lightUniformBuffers;
        std::vector<VkDeviceMemory>     lightUniformBuffersMemory;
        std::vector<VkBuffer>           nodeMaterialUniformBuffers;
        std::vector<VkDeviceMemory>     nodeMaterialUniformBuffersMemory;
        uint32_t                        nodeMaterialCount = 256;

        void Destroy(VkApplication  *app);
        void createPipeline(VkApplication *app, graphicsInfo info);
        void createDescriptorSetLayout(VkApplication *app);
        void createUniformBuffers(VkApplication *app, uint32_t imageCount);
        void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i);
    }second;

    void createColorAttachments();
    void createDepthAttachment();
    void createResolveAttachments();

    void oneSampleRenderPass();
    void multiSampleRenderPass();
    void oneSampleFrameBuffer();
    void multiSampleFrameBuffer();
public:
    graphics();
    void destroy();
    void destroyBuffers();
    void destroyCommandPools();

    void setApplication(VkApplication *app);
    void setImageProp(uint32_t imageCount, VkFormat imageFormat, VkExtent2D imageExtent, VkSampleCountFlagBits imageSamples);
    void setEmptyTexture(texture *emptyTexture);
    void setSkyboxTexture(cubeTexture * tex);

    void createAttachments();
    void createRenderPass();
    void createFramebuffers();
    void createPipelines();

    void createBaseDescriptorPool();
    void createBaseDescriptorSets();

    void createSkyboxDescriptorPool();
    void createSkyboxDescriptorSets();

    void createSecondDescriptorPool();
    void createSecondDescriptorSets(const std::vector<light<spotLight>*> & lightSources);

    void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i);
        void renderNode(Node* node, VkCommandBuffer& commandBuffer, VkDescriptorSet& descriptorSet, VkDescriptorSet& objectDescriptorSet, VkPipelineLayout& layout);
        void stencilRenderNode(Node* node, VkCommandBuffer& commandBuffer, VkDescriptorSet& descriptorSet, VkDescriptorSet& objectDescriptorSet, VkPipelineLayout& layout);
        void setMaterialNode(Node* node, std::vector<PushConstBlockMaterial> &nodeMaterials, uint32_t &objectPrimitive, const uint32_t firstPrimitive);

    void updateUniformBuffer(uint32_t currentImage, camera *cam);
    void updateSkyboxUniformBuffer(uint32_t currentImage, camera *cam);
    void updateMaterialUniformBuffer(uint32_t currentImage);
    void updateObjectUniformBuffer(uint32_t currentImage);
    void updateLightUniformBuffer(uint32_t currentImage, std::vector<light<spotLight> *> lightSource);

    void bindBaseObject(object *newObject);
    void bindBloomObject(object *newObject);
    void bindGodRaysObject(object *newObject);
    void bindStencilObject(object *newObject, float lineWidth, glm::vec4 lineColor);
    void bindSkyBoxObject(object *newObject);

    void setStencilObject(object *oldObject);

    std::vector<attachments>        & getAttachments();
    Objects                         getObjects();
};

class postProcessing
{
private:
    VkApplication                       *app;
    uint32_t                            imageCount;
    VkSampleCountFlagBits               msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    uint32_t                            swapChainAttachmentCount = 1;
    VkSwapchainKHR                      swapChain;
    std::vector<attachments>            swapChainAttachments;
    VkFormat                            swapChainImageFormat;
    VkExtent2D                          swapChainExtent;

    uint32_t                            AttachmentCount = 1;
    std::vector<attachments>            Attachments;

    VkRenderPass                        renderPass;
    std::vector<VkFramebuffer>          framebuffers;

    struct First{
        VkPipelineLayout                    PipelineLayout;
        VkPipeline                          Pipeline;
        VkDescriptorSetLayout               DescriptorSetLayout;
        VkDescriptorPool                    DescriptorPool;
        std::vector<VkDescriptorSet>        DescriptorSets;
    }first;

    struct Second{
        VkPipelineLayout                    PipelineLayout;
        VkPipeline                          Pipeline;
        VkDescriptorSetLayout               DescriptorSetLayout;
        VkDescriptorPool                    DescriptorPool;
        std::vector<VkDescriptorSet>        DescriptorSets;
    }second;

    //Создание цепочки обмена
    void createSwapChain(GLFWwindow* window, SwapChainSupportDetails swapChainSupport);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);    //Формат поверхности
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);     //Режим презентации
        VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);                              //Экстент - это разрешение изображений
    void createImageViews();
    void createColorAttachments();
public:
    postProcessing();
    void setApplication(VkApplication *app);
    void setMSAASamples(VkSampleCountFlagBits msaaSamples);
    void destroy();

    void createAttachments(GLFWwindow* window, SwapChainSupportDetails swapChainSupport);
    void createRenderPass();
    void createFramebuffers();
    void createPipelines();
        void createDescriptorSetLayout();
        void createFirstGraphicsPipeline();
        void createSecondGraphicsPipeline();

    void createDescriptorPool();
    void createDescriptorSets(std::vector<attachments> & Attachments);

    void render(std::vector<VkCommandBuffer> &commandBuffers, uint32_t i);

    VkSwapchainKHR                  & SwapChain();
    VkFormat                        & SwapChainImageFormat();
    VkExtent2D                      & SwapChainExtent();
    uint32_t                        & ImageCount();
};

#endif // GRAPHICS_H
