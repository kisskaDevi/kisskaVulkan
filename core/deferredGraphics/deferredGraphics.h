#ifndef DEFERREDGRAPHICS_H
#define DEFERREDGRAPHICS_H

#include "graphicsInterface.h"

#include "graphics.h"
#include "postProcessing.h"
#include "link.h"
#include "blur.h"
#include "customFilter.h"
#include "sslr.h"
#include "ssao.h"
#include "layersCombiner.h"
#include "scattering.h"
#include "skybox.h"
#include "shadow.h"
#include "boundingBox.h"

#include "device.h"
#include "buffer.h"

#include "vector.h"

#include <unordered_map>
#include <filesystem>

struct node;
class model;
class camera;

struct StorageBufferObject{
    alignas(16) vector<float,4>    mousePosition;
    alignas(4)  int                number;
    alignas(4)  float              depth;
};

class deferredGraphics: public graphicsInterface{
private:
    std::filesystem::path                       shadersPath;
    uint32_t                                    imageCount{0};
    VkExtent2D                                  extent{0,0};
    VkOffset2D                                  offset{0,0};
    VkExtent2D                                  frameBufferExtent{0,0};
    VkSampleCountFlagBits                       MSAASamples{VK_SAMPLE_COUNT_1_BIT};

    std::vector<physicalDevice>                 devices;
    physicalDevice                              device;

    swapChain*                                  swapChainKHR{nullptr};

    DeferredAttachments                         deferredAttachments;
    std::vector<DeferredAttachments>            transparentLayersAttachments;
    attachments                                 blurAttachment;
    attachments                                 sslrAttachment;
    attachments                                 ssaoAttachment;
    attachments                                 scatteringAttachment;
    attachments                                 boundingBoxAttachment;
    std::vector<attachments>                    blitAttachments;
    skyboxAttachments                           skyboxAttachment;
    layersCombinerAttachments                   combinedAttachment;
    attachments                                 finalAttachment;

    graphics                                    DeferredGraphics;
    gaussianBlur                                Blur;
    customFilter                                Filter;
    SSLRGraphics                                SSLR;
    SSAOGraphics                                SSAO;
    skyboxGraphics                              Skybox;
    shadowGraphics                              Shadow;
    layersCombiner                              LayersCombiner;
    scattering                                  Scattering;
    postProcessingGraphics                      PostProcessing;
    boundingBoxGraphics                         BoundingBox;

    link                                        Link;
    std::vector<graphics>                       TransparentLayers;

    bool                                        enableTransparentLayers{false};
    bool                                        enableSkybox{false};
    bool                                        enableBlur{false};
    bool                                        enableBloom{false};
    bool                                        enableSSLR{false};
    bool                                        enableSSAO{false};
    bool                                        enableScattering{false};
    bool                                        enableShadow{false};
    bool                                        enableBoundingBox{false};

    std::vector<buffer>                         storageBuffersHost;

    VkCommandPool                               commandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer>                copyCommandBuffers;
    std::vector<bool>                           updateCommandBufferFlags;
    std::vector<node*>                          nodes;

    float                                       blitFactor{1.5f};
    uint32_t                                    blitAttachmentCount{8};
    uint32_t                                    TransparentLayersCount{2};

    camera*                                     cameraObject{nullptr};
    std::unordered_map<std::string, texture*>   emptyTextures;

    void createStorageBuffers(uint32_t imageCount);
    void createGraphicsPasses();
    void createCommandBuffers();
    void createCommandPool();
    void createEmptyTexture();

    void freeCommandBuffers();
    void destroyCommandPool();
    void destroyEmptyTextures();
public:
    deferredGraphics(const std::filesystem::path& shadersPath, VkExtent2D extent, VkOffset2D offset = {0,0}, VkSampleCountFlagBits MSAASamples = VK_SAMPLE_COUNT_1_BIT);
    ~deferredGraphics() = default;

    void destroyGraphics() override;

    void setDevices(uint32_t devicesCount, physicalDevice* devices) override;
    void setSwapChain(swapChain* swapChainKHR) override;

    void createGraphics() override;

    void updateDescriptorSets();
    void updateCommandBuffer(uint32_t imageIndex) override;
    void updateBuffers(uint32_t imageIndex) override;

    std::vector<std::vector<VkSemaphore>> submit(
        const std::vector<std::vector<VkSemaphore>>& externalSemaphore,
        const std::vector<VkFence>& externalFence,
        uint32_t imageIndex) override;

    linkable* getLinkable() override;

    void        updateCmdFlags();

    void        setExtentAndOffset(VkExtent2D extent, VkOffset2D offset = {0,0});
    void        setFrameBufferExtent(VkExtent2D extent);
    void        setShadersPath(const std::filesystem::path& shadersPath);
    void        setMinAmbientFactor(const float& minAmbientFactor);
    void        setScatteringRefraction(bool enable);

    void        create(model* pModel);
    void        destroy(model* pModel);

    void        bind(camera* cameraObject);
    void        remove(camera* cameraObject);

    void        bind(object* object);
    bool        remove(object* object);

    void        bind(light* lightSource);
    void        remove(light* lightSource);

    void        updateStorageBuffer(uint32_t currentImage, const float& mousex, const float& mousey);
    uint32_t    readStorageBuffer(uint32_t currentImage);

    deferredGraphics& setEnableTransparentLayers(bool enable);
    deferredGraphics& setEnableSkybox(bool enable);
    deferredGraphics& setEnableBlur(bool enable);
    deferredGraphics& setEnableBloom(bool enable);
    deferredGraphics& setEnableSSLR(bool enable);
    deferredGraphics& setEnableSSAO(bool enable);
    deferredGraphics& setEnableScattering(bool enable);
    deferredGraphics& setEnableShadow(bool enable);
    deferredGraphics& setEnableBoundingBox(bool enable);
};

#endif // DEFERREDGRAPHICS_H
