#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Config.hpp"

class VulkanBase
{
public:
    // Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice) { mPhysicalDevice = physicalDevice; }
    void setLogicalDevice(VkDevice &device) { mLogicalDevice = device; }
    void setCommandPool(VkCommandPool &commandPool) { mCommandPool = commandPool; }
    void setGraphicsQueue(VkQueue graphicsQueue) { mGraphicsQueue = graphicsQueue; }
    void setFramebufferResolution(int fbWidth, int fbHeight) { mFbWidth = fbWidth; mFbHeight = fbHeight; }

    // Getters
    VkBuffer& getVertexBuffer() { return mVertexBuffer; }
    VkDeviceMemory& getVertexBufferMemory() { return mVertexBufferMemory; }
    VkBuffer& getIndexBuffer() { return mIndexBuffer; }
    VkDeviceMemory& getIndexBufferMemory() { return mIndexBufferMemory; }
    VkPipeline& getPipeline() { return mPipeline; }
    VkPipelineLayout& getPipelineLayout() { return mPipelineLayout; }

protected:
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mLogicalDevice;
    VkCommandPool mCommandPool;
    VkQueue mGraphicsQueue;
    
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    VkPipeline mPipeline;
    VkPipelineLayout mPipelineLayout;
    
    int mFbWidth = WIDTH;
    int mFbHeight = HEIGHT;
};