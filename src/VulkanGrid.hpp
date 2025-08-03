#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Config.hpp"

struct GridPushConstants
{
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanGrid
{
  public:
    void createPipeline(VkRenderPass &renderPass);
    void createVertexBuffer();

    // Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice){ mPhysicalDevice = physicalDevice; }
    void setLogicalDevice(VkDevice &device){ mLogicalDevice = device; }
    void setCommandPool(VkCommandPool &commandPool){ mCommandPool = commandPool; }
    /// VulkanGrid needs the graphics queue to transfer a staging buffer into device-local buffer
    void setGraphicsQueue(VkQueue graphicsQueue){ mGraphicsQueue = graphicsQueue; }
    void setFramebufferResolution(int fbWidth, int fbHeight){ mFbWidth = fbWidth; mFbHeight = fbHeight; }

    // Getters
    VkPipelineLayout& getPipelineLayout(){ return mPipelineLayout; }
    VkPipeline& getPipeline(){ return mPipeline; }
    VkBuffer& getVertexBuffer(){ return mVertexBuffer; }
    uint32_t getVertexCount(){ return mVertexCount; }
    VkDeviceMemory& getVertexBufferMemory(){ return mVertexBufferMemory; }

  private:
    int generateLines(int halfExtent, float spacing, glm::vec3 lines[]);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  
    int mFbWidth = WIDTH;
    int mFbHeight = HEIGHT;

    VkPhysicalDevice mPhysicalDevice;
    VkDevice mLogicalDevice;
  
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    uint32_t mVertexCount;

    VkCommandPool mCommandPool;
    VkQueue mGraphicsQueue;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mPipeline;
};
