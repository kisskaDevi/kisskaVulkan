#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#define STB_IMAGE_IMPLEMENTATION

#include "dualQuaternion.h"
#include "graphicsManager.h"
#include "deferredGraphics.h"
#include "baseCamera.h"
#include "scene.h"

#include "stb_image.h"
#include <glfw3.h>
#include <glm.hpp>
#include <chrono>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <utility>
#include <filesystem>

#include <vector.h>
#include <matrix.h>

bool framebufferResized = false;

GLFWwindow* initializeWindow(uint32_t WIDTH, uint32_t HEIGHT, std::filesystem::path iconName = "");
std::pair<uint32_t,uint32_t> resize(GLFWwindow* window, graphicsManager* app, std::vector<deferredGraphics*> graphics, baseCamera* cameraObject);

int main()
{
    float fps = 60.0f;
    bool fpsLock = false;
    uint32_t WIDTH = 800;
    uint32_t HEIGHT = 800;
    const std::filesystem::path ExternalPath = std::filesystem::absolute(std::string(__FILE__) + "/../../");

    GLFWwindow* window = initializeWindow(WIDTH, HEIGHT, ExternalPath / "dependences/texture/icon.png");

    std::vector<deferredGraphics*> graphics = {
          new deferredGraphics{ExternalPath / "core/deferredGraphics/shaders", {WIDTH, HEIGHT}}
        , new deferredGraphics{ExternalPath / "core/deferredGraphics/shaders", {WIDTH/3, HEIGHT/3}, {static_cast<int32_t>(WIDTH / 2), static_cast<int32_t>(HEIGHT / 2)}}
    };

    graphicsManager app;
    debug::checkResult(app.createSurface(window), "in file " + std::string(__FILE__) + ", line " + std::to_string(__LINE__));
    debug::checkResult(app.createDevice(), "in file " + std::string(__FILE__) + ", line " + std::to_string(__LINE__));
    debug::checkResult(app.createSwapChain(window), "in file " + std::string(__FILE__) + ", line " + std::to_string(__LINE__));
    debug::checkResult(app.createSyncObjects(), "in file " + std::string(__FILE__) + ", line " + std::to_string(__LINE__));

    baseCamera cameraObject(45.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 500.0f);
    cameraObject.translate(glm::vec3(0.0f,0.0f,15.0f));

    for(auto& graph: graphics){
        app.setGraphics(graph);
        graph->createCommandPool();
        graph->createEmptyTexture();

        graph->bindCameraObject(&cameraObject, &graph == &graphics[0]);
        graph->createGraphics(window, &app.getSurface());
    }

    scene testScene(&app, graphics, window, ExternalPath);
    testScene.createScene(WIDTH,HEIGHT,&cameraObject);

    static auto pastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - pastTime).count();

        if(fpsLock && fps < 1.0f/frameTime) continue;
        pastTime = std::chrono::high_resolution_clock::now();

        glfwSetWindowTitle(window, std::stringstream("Vulkan [" + std::to_string(1.0f/frameTime) + " FPS]").str().c_str());

        if(app.checkNextFrame() != VK_ERROR_OUT_OF_DATE_KHR)
        {
            testScene.updateFrame(app.getImageIndex(),frameTime,WIDTH,HEIGHT);

            if (VkResult result = app.drawFrame(); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized){
                framebufferResized = false;
                std::tie(WIDTH, HEIGHT) = resize(window,&app,graphics,&cameraObject);
            } else if(result) {
                throw std::runtime_error("failed to with " + std::to_string(result));
            }
        }
    }

    debug::checkResult(app.deviceWaitIdle(), "in file " + std::string(__FILE__) + ", line " + std::to_string(__LINE__));

    testScene.destroyScene();
    for(auto& graph: graphics){
        graph->destroyGraphics();
        graph->destroyEmptyTextures();
        graph->destroyCommandPool();
        graph->removeCameraObject(&cameraObject);
        delete graph;
    }
    app.destroySwapChain();
    app.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

std::pair<uint32_t,uint32_t> resize(GLFWwindow* window, graphicsManager* app, std::vector<deferredGraphics*> graphics, baseCamera* cameraObject)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width * height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    cameraObject->recreate(45.0f, (float) width / (float) height, 0.1f, 500.0f);
    graphics[0]->setExtentAndOffset({static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
    graphics[1]->setExtentAndOffset({static_cast<uint32_t>(width / 3), static_cast<uint32_t>(height / 3)}, {static_cast<int32_t>(width / 2), static_cast<int32_t>(height / 2)});

    app->deviceWaitIdle();
    app->destroySwapChain();
    app->createSwapChain(window);

    for(auto& graph: graphics){
        graph->destroyGraphics();
        graph->createGraphics(window, &app->getSurface());
    }

    return std::pair<uint32_t,uint32_t>(width, height);
}

GLFWwindow* initializeWindow(uint32_t WIDTH, uint32_t HEIGHT, std::filesystem::path iconName)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, nullptr);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int, int){ framebufferResized = true;});

    if(iconName.string().size() > 0){
        int width, height, comp;
        stbi_uc* img = stbi_load(iconName.string().c_str(), &width, &height, &comp, 0);
        GLFWimage images{width,height,img};
        glfwSetWindowIcon(window,1,&images);
    }

    return window;
}
