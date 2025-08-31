#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanBase
{
public:
    // Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice) { mPhysicalDevice = physicalDevice; }
    void setLogicalDevice(VkDevice &device) { mLogicalDevice = device; }
    void setCommandPool(VkCommandPool &commandPool) { mCommandPool = commandPool; }
    void setGraphicsQueue(VkQueue graphicsQueue) { mGraphicsQueue = graphicsQueue; }

    // Getters
    VkBuffer& getVertexBuffer() { return mVertexBuffer; }
    VkDeviceMemory& getVertexBufferMemory() { return mVertexBufferMemory; }
    VkPipeline& getPipeline() { return mPipeline; }
    VkPipelineLayout& getPipelineLayout() { return mPipelineLayout; }

protected:
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mLogicalDevice;
    VkCommandPool mCommandPool;
    VkQueue mGraphicsQueue;
    
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkPipeline mPipeline;
    VkPipelineLayout mPipelineLayout;
};