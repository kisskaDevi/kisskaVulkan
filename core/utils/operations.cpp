#include "operations.h"
#include <glfw3.h>

#include <unordered_map>
#include <set>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string.h>


VkResult debug::errorResult(const std::string& message){
#ifndef NDEBUG
    std::cerr << "ERROR : " << message << std::endl;
#endif
    return VK_ERROR_UNKNOWN;
}

void debug::checkResult(VkResult result, std::string message){
    if (result != VK_SUCCESS){
#ifndef NDEBUG
        std::cerr << "ERROR [" << result <<"] :: " << message << std::endl;
#endif
#ifdef THROW_EXEPTION
        throw std::runtime_error(message);
#endif
    }
}

bool debug::checkResult(bool result, std::string message){
    if (result){
#ifndef NDEBUG
        std::cerr << "ERROR :: " << message << std::endl;
#endif
#ifdef THROW_EXEPTION
        throw std::runtime_error(message);
#endif
    }
    return result;
}


#define ONLYDEVICELOCALHEAP

bool ValidationLayer::checkSupport(const std::vector<const char*> validationLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool res = true;
    for (const char* layerName : validationLayers){
        bool layerFound = false;
        for(const auto& layerProperties: availableLayers){
            layerFound |= (strcmp(layerName, layerProperties.layerName) == 0);
        }
        res &= layerFound;
    }
    return res;
}

void ValidationLayer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
if(func != nullptr) func(instance, debugMessenger, pAllocator);
}

void ValidationLayer::setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT* debugMessenger)
{
auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
if(func != nullptr) func(instance, &createInfo, nullptr, debugMessenger);
}

VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayer::debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    static_cast<void>(messageSeverity);
    static_cast<void>(messageType);
    static_cast<void>(pUserData);
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void PhysicalDevice::printMemoryProperties(VkPhysicalDeviceMemoryProperties memoryProperties){
    std::cout << "memoryHeapCount = " << memoryProperties.memoryHeapCount << std::endl;
    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++){
        std::cout << "heapFlag[" << i << "] = " << memoryProperties.memoryHeaps[i].flags << "\t\t"
                  << "heapSize[" << i << "] = " << memoryProperties.memoryHeaps[i].size << std::endl;
    }
    std::cout << "memoryTypeCount = " << memoryProperties.memoryTypeCount << std::endl;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++){
        std::cout << "heapIndex[" << i << "] = " << memoryProperties.memoryTypes[i].heapIndex << '\t'
                  << "heapType [" << i << "] = " << memoryProperties.memoryTypes[i].propertyFlags << std::endl;
    }
    std::cout<<std::endl;
}

uint32_t PhysicalDevice::findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

#ifdef ONLYDEVICELOCALHEAP
    uint32_t deviceLocalHeapIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++){
        if(memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT){
            deviceLocalHeapIndex = i;
            break;
        }
    }
#endif

    std::vector<uint32_t> memoryTypeIndex;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++){
        if ((memoryTypeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties){
#ifdef ONLYDEVICELOCALHEAP
            if(memoryProperties.memoryTypes[i].heapIndex == deviceLocalHeapIndex)
#endif
                memoryTypeIndex.push_back(i);
        }
    }
    return memoryTypeIndex.size() != 0 ? memoryTypeIndex[0] : UINT32_MAX;
}

void PhysicalDevice::printQueueIndices(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t index = 0; index < queueFamilyCount; index++){
        VkBool32 presentSupport = false;
        if(surface){
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
        }

        std::cout << "index\t" << index << "\tqueue count\t" << queueFamilies[index].queueCount << std::endl;
        if(queueFamilies[index].queueFlags & VK_QUEUE_GRAPHICS_BIT){
            std::cout << "index\t" << index << "\t:\t" << "VK_QUEUE_GRAPHICS_BIT" << std::endl;
        }
        if(queueFamilies[index].queueFlags & VK_QUEUE_COMPUTE_BIT){
            std::cout << "index\t" << index << "\t:\t" << "VK_QUEUE_COMPUTE_BIT" << std::endl;
        }
        if(queueFamilies[index].queueFlags & VK_QUEUE_TRANSFER_BIT){
            std::cout << "index\t" << index << "\t:\t" << "VK_QUEUE_TRANSFER_BIT" << std::endl;
        }
        if(queueFamilies[index].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT){
            std::cout << "index\t" << index << "\t:\t" << "VK_QUEUE_SPARSE_BINDING_BIT" << std::endl;
        }
        if(queueFamilies[index].queueFlags & VK_QUEUE_PROTECTED_BIT){
            std::cout << "index\t" << index << "\t:\t" << "VK_QUEUE_PROTECTED_BIT" << std::endl;
        }
        if(presentSupport){
            std::cout << "index\t" << index << "\t:\t" << "Present Support" << std::endl;
        }
        std::cout << std::endl;
    }
}

std::vector<uint32_t> PhysicalDevice::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    std::vector<uint32_t> indices;

    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, queueFamilies.data());

    for (uint32_t index = 0; index < queueFamilyPropertyCount; index++){
        VkBool32 presentSupport = surface ? false : true;
        if(surface){
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
        }
        if (presentSupport){
            indices.push_back(index);
        }
    }

    return indices;
}

std::vector<uint32_t> PhysicalDevice::findQueueFamilies(VkPhysicalDevice device, VkQueueFlagBits queueFlags, VkSurfaceKHR surface)
{
    std::vector<uint32_t> indices;

    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, queueFamilies.data());

    for (uint32_t index = 0; index < queueFamilyPropertyCount; index++){
        VkBool32 presentSupport = surface ? false : true;
        if(surface){
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
        }
        if ((queueFamilies[index].queueFlags & queueFlags) == queueFlags && presentSupport){
            indices.push_back(index);
        }
    }

    return indices;
}

std::vector<VkQueueFamilyProperties> PhysicalDevice::findQueueFamiliesProperties(VkPhysicalDevice device, VkQueueFlagBits queueFlags, VkSurfaceKHR surface)
{
    std::vector<VkQueueFamilyProperties> result;

    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertyCount, queueFamilies.data());

    for (uint32_t index = 0; index < queueFamilyPropertyCount; index++){
        VkBool32 presentSupport = surface ? false : true;
        if(surface){
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
        }
        if ((queueFamilies[index].queueFlags & queueFlags) == queueFlags && presentSupport){
            result.push_back(queueFamilies[index]);
        }
    }

    return result;
}

VkSampleCountFlagBits PhysicalDevice::queryingSampleCount(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

bool PhysicalDevice::isSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions)
{
    VkPhysicalDeviceFeatures supportedFeatures{};
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return PhysicalDevice::isExtensionsSupport(device, deviceExtensions) && SwapChain::queryingSupport(device, surface).isNotEmpty() && supportedFeatures.samplerAnisotropy;
}

bool PhysicalDevice::isExtensionsSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VkResult Buffer::create(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
    VkResult result = VK_SUCCESS;

    VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    result = vkCreateBuffer(device, &bufferInfo, nullptr, buffer);
    debug::checkResult(result, "VkBuffer : vkCreateBuffer result = " + std::to_string(result));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = PhysicalDevice::findMemoryTypeIndex(physicalDevice, memoryRequirements.memoryTypeBits, properties);
    result = vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory);
    debug::checkResult(result, "VkDeviceMemory : vkAllocateMemory result = " + std::to_string(result));

    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);

    return result;
}

void Buffer::copy(VkCommandBuffer commandBuffer, VkDeviceSize size, VkBuffer srcBuffer, VkBuffer dstBuffer)
{
    VkBufferCopy copyRegion{};
        copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
}

VkCommandBuffer SingleCommandBuffer::create(VkDevice device, VkCommandPool commandPool)
{
    VkResult result = VK_SUCCESS;
    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
    result = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    debug::checkResult(result, "VkCommandBuffer : vkAllocateCommandBuffers result = " + std::to_string(result));


    VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
    result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    debug::checkResult(result, "VkCommandBuffer : vkBeginCommandBuffer result = " + std::to_string(result));

    return commandBuffer;
}

VkResult SingleCommandBuffer::submit(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer* commandBuffer)
{
    VkResult result = vkEndCommandBuffer(*commandBuffer);
    debug::checkResult(result, "VkCommandPool : vkEndCommandBuffer result = " + std::to_string(result));

    VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer;
    result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    debug::checkResult(result, "VkSubmitInfo : vkQueueSubmit result = " + std::to_string(result));

    result = vkQueueWaitIdle(queue);
    debug::checkResult(result, "VkQueue : vkQueueWaitIdle result = " + std::to_string(result));

    vkFreeCommandBuffers(device, commandPool, 1, commandBuffer);

    return result;
}

void Texture::transitionLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t baseArrayLayer, uint32_t arrayLayers){
    std::unordered_map<VkImageLayout,std::pair<VkAccessFlags,VkPipelineStageFlags>> layoutDescription;
    layoutDescription[VK_IMAGE_LAYOUT_UNDEFINED] = {0,VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
    layoutDescription[VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL] = {VK_ACCESS_TRANSFER_WRITE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT};
    layoutDescription[VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL] = {VK_ACCESS_TRANSFER_READ_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT};
    layoutDescription[VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL] = {VK_ACCESS_SHADER_READ_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT};

    VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
        barrier.subresourceRange.layerCount = arrayLayers;
        barrier.srcAccessMask = layoutDescription[oldLayout].first;
        barrier.dstAccessMask = layoutDescription[newLayout].first;
    vkCmdPipelineBarrier(commandBuffer, layoutDescription[oldLayout].second, layoutDescription[newLayout].second, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Texture::copyFromBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, VkExtent3D extent, uint32_t layerCount){
    VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = extent;
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

VkResult Texture::create(VkPhysicalDevice physicalDevice, VkDevice device, VkImageCreateFlags flags, VkExtent3D extent, uint32_t arrayLayers, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory){
    VkResult result = VK_SUCCESS;

    VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = flags;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = extent;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = arrayLayers;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = layout;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    result = vkCreateImage(device, &imageInfo, nullptr, image);
    debug::checkResult(result, "VkImage : vkCreateImage result = " + std::to_string(result));

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = PhysicalDevice::findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits, properties);
    result = vkAllocateMemory(device, &allocInfo, nullptr, imageMemory);
    debug::checkResult(result, "VkDeviceMemory : vkAllocateMemory result = " + std::to_string(result));

    result = vkBindImageMemory(device, *image, *imageMemory, 0);
    debug::checkResult(result, "VkImage : vkBindImageMemory result = " + std::to_string(result));

    return result;
}

VkResult Texture::createView(VkDevice device, VkImageViewType type, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t baseArrayLayer, uint32_t layerCount, VkImage image, VkImageView* imageView)
{
    VkResult result = VK_SUCCESS;

    VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = type;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
        viewInfo.subresourceRange.layerCount = layerCount;
    result = vkCreateImageView(device, &viewInfo, nullptr, imageView);
    debug::checkResult(result, "VkImageView : vkCreateImageView result = " + std::to_string(result));

    return result;
}

void Texture::generateMipmaps(VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t baseArrayLayer, uint32_t layerCount)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)){
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
        barrier.subresourceRange.layerCount = layerCount;
        barrier.subresourceRange.levelCount = 1;

    for (uint32_t i = 1, mipWidth = texWidth, mipHeight = texHeight; i < mipLevels; i++, mipWidth /= 2, mipHeight /= 2) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        blitDown(commandBuffer,image,i - 1,image,i,(mipWidth > 1 ? mipWidth : 1),(mipHeight > 1 ? mipHeight : 1),baseArrayLayer,layerCount,2);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }
        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Texture::blitDown(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcMipLevel, VkImage dstImage, uint32_t dstMipLevel, uint32_t width, uint32_t height, uint32_t baseArrayLayer, uint32_t layerCount, float blitFactor)
{
    VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {static_cast<int32_t>(width),static_cast<int32_t>(height),1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = srcMipLevel;
        blit.srcSubresource.baseArrayLayer = baseArrayLayer;
        blit.srcSubresource.layerCount = layerCount;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {static_cast<int32_t>((width/blitFactor > 1) ? (width/blitFactor) : 1),static_cast<int32_t>((height/blitFactor > 1) ? (height/blitFactor) : 1), 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = dstMipLevel;
        blit.dstSubresource.baseArrayLayer = baseArrayLayer;
        blit.dstSubresource.layerCount = layerCount;
    vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
}

void Texture::blitUp(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcMipLevel, VkImage dstImage, uint32_t dstMipLevel, uint32_t width, uint32_t height, uint32_t baseArrayLayer, uint32_t layerCount, float blitFactor)
{
    VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {static_cast<int32_t>(width/blitFactor),static_cast<int32_t>(height/blitFactor),1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = srcMipLevel;
        blit.srcSubresource.baseArrayLayer = baseArrayLayer;
        blit.srcSubresource.layerCount = layerCount;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {static_cast<int32_t>(width),static_cast<int32_t>(height),1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = dstMipLevel;
        blit.dstSubresource.baseArrayLayer = baseArrayLayer;
        blit.dstSubresource.layerCount = layerCount;
    vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
}

SwapChain::SupportDetails SwapChain::queryingSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChain::SupportDetails details{};
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    debug::checkResult(result, "VkSurfaceKHR : vkGetPhysicalDeviceSurfaceCapabilitiesKHR result = " + std::to_string(result));

    uint32_t formatCount = 0, presentModeCount = 0;

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    debug::checkResult(result, "VkSurfaceKHR : vkGetPhysicalDeviceSurfaceFormatsKHR result = " + std::to_string(result));
    if (formatCount != 0){
        details.formats.resize(formatCount);
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        debug::checkResult(result, "VkSurfaceKHR : vkGetPhysicalDeviceSurfaceFormatsKHR result = " + std::to_string(result));
    }

    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    debug::checkResult(result, "VkSurfaceKHR : vkGetPhysicalDeviceSurfacePresentModesKHR result = " + std::to_string(result));
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        debug::checkResult(result, "VkSurfaceKHR : vkGetPhysicalDeviceSurfacePresentModesKHR result = " + std::to_string(result));
    }

    return details;
}

uint32_t SwapChain::queryingSupportImageCount(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    auto capabilities = SwapChain::queryingSupport(device, surface).capabilities;
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount){
        imageCount = capabilities.maxImageCount;
    }
    return imageCount;
}

VkSurfaceFormatKHR SwapChain::queryingSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR SwapChain::queryingPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::queryingExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = (capabilities.currentExtent.width != UINT32_MAX && capabilities.currentExtent.height != UINT32_MAX) ? capabilities.currentExtent :
    VkExtent2D{ actualExtent.width = std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                actualExtent.height = std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};

    return actualExtent;
}

VkFormat Image::supportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    VkFormat supportedFormat = candidates[0];
    for (VkFormat format : candidates)
    {
        VkFormatProperties props{};
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if ((tiling == VK_IMAGE_TILING_OPTIMAL || tiling == VK_IMAGE_TILING_LINEAR) && (props.linearTilingFeatures & features) == features){
            supportedFormat = format; break;
        }
    }
    return supportedFormat;
}

VkFormat Image::depthStencilFormat(VkPhysicalDevice physicalDevice)
{
    return Image::supportedFormat(
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

std::vector<char> ShaderModule::readFile(const std::filesystem::path& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed to open file " << filename.string() << std::endl;
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule ShaderModule::create(VkDevice* device, const std::vector<char>& code)
{
    VkShaderModule shaderModule{VK_NULL_HANDLE};
    VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkResult result = vkCreateShaderModule(*device, &createInfo, nullptr, &shaderModule);
    debug::checkResult(result, "VkShaderModule : vkCreateShaderModule result = " + std::to_string(result));

    return shaderModule;
}
