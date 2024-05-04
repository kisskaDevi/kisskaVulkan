#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "transformational.h"
#include "light.h"
#include "quaternion.h"
#include "buffer.h"

#include <filesystem>

class shadowGraphics;
namespace moon::utils { class Texture;}

struct LightBufferObject
{
    alignas(16) matrix<float,4,4>   proj;
    alignas(16) matrix<float,4,4>   view;
    alignas(16) matrix<float,4,4>   projView;
    alignas(16) vector<float,4>     position;
    alignas(16) vector<float,4>     lightColor;
    alignas(16) vector<float,4>     lightProp;
};

enum spotType
{
    circle,
    square
};

class spotLight : public transformational, public light
{
private:
    moon::utils::Texture*               tex{nullptr};
    moon::utils::Texture*               emptyTextureBlack{nullptr};
    moon::utils::Texture*               emptyTextureWhite{nullptr};

    float                               lightPowerFactor{10.0f};
    float                               lightDropFactor{1.0f};
    vector<float,4>                     lightColor{0.0f};

    matrix<float,4,4>                   projectionMatrix{1.0f};
    bool                                created{false};
    VkDevice                            device{VK_NULL_HANDLE};
    spotType                            type{spotType::circle};

    quaternion<float>                   translation{0.0f,0.0f,0.0f,0.0f};
    quaternion<float>                   rotation{1.0f,0.0f,0.0f,0.0f};
    quaternion<float>                   rotationX{1.0f,0.0f,0.0f,0.0f};
    quaternion<float>                   rotationY{1.0f,0.0f,0.0f,0.0f};
    vector<float,3>                     scaling{1.0f,1.0f,1.0f};
    matrix<float,4,4>                   globalTransformation{1.0f};
    matrix<float,4,4>                   modelMatrix{1.0f};

    VkDescriptorSetLayout               textureDescriptorSetLayout{VK_NULL_HANDLE};
    std::vector<VkDescriptorSet>        textureDescriptorSets;

    std::vector<moon::utils::Buffer> uniformBuffersHost;
    std::vector<moon::utils::Buffer> uniformBuffersDevice;

    void createUniformBuffers(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t imageCount);
    void createDescriptorPool(VkDevice device, uint32_t imageCount);
    void createDescriptorSets(VkDevice device, uint32_t imageCount);
    void updateDescriptorSets(VkDevice device, uint32_t imageCount);

    void updateUniformBuffersFlags(std::vector<moon::utils::Buffer>& uniformBuffers);
    void updateModelMatrix();
public:
    spotLight(const vector<float,4>& color, const matrix<float,4,4> & projection, bool enableShadow = true, bool enableScattering = false, spotType type = spotType::circle);
    spotLight(const std::filesystem::path & TEXTURE_PATH, const matrix<float,4,4> & projection, bool enableShadow = true, bool enableScattering = false, spotType type = spotType::circle);
    virtual ~spotLight();

    spotLight&          setGlobalTransform(const matrix<float,4,4> & transform) override;
    spotLight&          translate(const vector<float,3> & translate) override;
    spotLight&          rotate(const float & ang,const vector<float,3> & ax) override;
    spotLight&          scale(const vector<float,3> & scale) override;

    spotLight&          rotateX(const float & ang ,const vector<float,3> & ax);
    spotLight&          rotateY(const float & ang ,const vector<float,3> & ax);
    spotLight&          setTranslation(const vector<float,3>& translate);
    spotLight&          setRotation(const quaternion<float>& rotation);
    spotLight&          setRotation(const float & ang ,const vector<float,3> & ax);
    spotLight&          rotate(const quaternion<float>& quat);

    void                setLightColor(const vector<float,4> & color);
    void                setLightDropFactor(const float& dropFactor);
    void                setTexture(moon::utils::Texture* tex);
    void                setProjectionMatrix(const matrix<float,4,4> & projection);

    matrix<float,4,4>   getModelMatrix() const;
    vector<float,3>     getTranslate() const;
    vector<float,4>     getLightColor() const;

    void destroy(VkDevice device) override;

    void create(
            moon::utils::PhysicalDevice device,
            VkCommandPool commandPool,
            uint32_t imageCount) override;

    void render(
        uint32_t frameNumber,
        VkCommandBuffer commandBuffer,
        const std::vector<VkDescriptorSet>& descriptorSet,
        VkPipelineLayout pipelineLayout,
        VkPipeline pipeline) override;

    void update(
        uint32_t frameNumber,
        VkCommandBuffer commandBuffer) override;

    void printStatus() const;
};

class isotropicLight: public transformational
{
private:
    vector<float,4>                     lightColor{0.0f};
    matrix<float,4,4>                   projectionMatrix{1.0f};
    float                               lightDropFactor{1.0f};

    quaternion<float>                   translation{0.0f,0.0f,0.0f,0.0f};
    quaternion<float>                   rotation{1.0f,0.0f,0.0f,0.0f};
    quaternion<float>                   rotationX{1.0f,0.0f,0.0f,0.0f};
    quaternion<float>                   rotationY{1.0f,0.0f,0.0f,0.0f};
    vector<float,3>                     scaling{1.0f,1.0f,1.0f};
    matrix<float,4,4>                   globalTransformation{1.0f};
    matrix<float,4,4>                   modelMatrix{1.0f};

    std::vector<spotLight*>             lightSource;

    void updateModelMatrix();
public:
    isotropicLight(const vector<float,4>& color, float radius = 100.0f);
    ~isotropicLight();

    void setLightColor(const vector<float,4> & color);
    void setLightDropFactor(const float& dropFactor);
    void setProjectionMatrix(const matrix<float,4,4> & projection);
    void setTranslation(const vector<float,3>& translate);

    isotropicLight& setGlobalTransform(const matrix<float,4,4>& transform);
    isotropicLight& translate(const vector<float,3>& translate);
    isotropicLight& rotate(const float& ang,const vector<float,3>& ax);
    isotropicLight& scale(const vector<float,3>& scale);

    void rotateX(const float& ang ,const vector<float,3>& ax);
    void rotateY(const float& ang ,const vector<float,3>& ax);

    vector<float,3> getTranslate() const;
    vector<float,4> getLightColor() const;
    std::vector<spotLight*> get() const;
};

#endif // SPOTLIGHT_H
