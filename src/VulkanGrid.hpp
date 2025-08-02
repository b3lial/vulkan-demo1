#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Config.hpp"

class VulkanGrid
{
  public:
    void createGridPipeline(VkRenderPass &renderPass);
    void createGridVertexBuffer();

    // Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice)
    {
      mPhysicalDevice = physicalDevice;
    }

    void setLogicalDevice(VkDevice &device)
    {
      mDevice = device;
    }

    void setCommandPool(VkCommandPool &commandPool)
    {
      mCommandPool = commandPool;
    }

    /// VulkanGrid needs the graphica queue to transfer a staging buffer into device-local buffer
    void setGraphicsQueue(VkQueue graphicsQueue)
    {
      mGraphicsQueue = graphicsQueue;
    }

    void setFramebufferResolution(int fbWidth, int fbHeight)
    {
      mFbWidth = fbWidth;
      mFbHeight = fbHeight;
    }

    // Getters
    VkPipelineLayout& getPipelineLayout()
    {
      return mPipelineLayout;
    }

    VkPipeline& getPipeline()
    {
      return mPipeline;
    }

    VkBuffer& getVertexBuffer()
    {
      return mVertexBuffer;
    }

    uint32_t getVertexCount()
    {
      return mVertexCount;
    }

    VkDeviceMemory& getVertexBufferMemory()
    {
      return mVertexBufferMemory;
    }

  private:
    int generateLines(int halfExtent, float spacing, glm::vec3 lines[]);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    VkCommandPool mCommandPool;
    VkQueue mGraphicsQueue;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mPipeline;
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    uint32_t mVertexCount;

    int mFbWidth = WIDTH;
    int mFbHeight = HEIGHT;
};
