#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class VulkanGrid
{
  public:
    void createGridPipeline(VkRenderPass &renderPass);
    void createGridVertexBuffer();

    // Plain Getters and Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice)
    {
      mPhysicalDevice = physicalDevice;
    }

    void setDevice(VkDevice &device)
    {
      mDevice = device;
    }

    void setCommandPool(VkCommandPool &commandPool)
    {
      mCommandPool = commandPool;
    }

    void setGraphicsQueue(VkQueue graphicsQueue)
    {
      mGraphicsQueue = graphicsQueue;
    }

    VkPipelineLayout& getPipelineLayout()
    {
      return gridPipelineLayout;
    }

    VkPipeline& getPipeline()
    {
      return gridPipeline;
    }

    VkBuffer& getVertexBuffer()
    {
      return gridVertexBuffer;
    }

    uint32_t getVertexCount()
    {
      return gridVertexCount;
    }

    VkDeviceMemory& getVertexBufferMemory()
    {
      return gridVertexBufferMemory;
    }

  private:
    int generateGridLines(int halfExtent, float spacing, glm::vec3 lines[]);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    VkCommandPool mCommandPool;
    VkQueue mGraphicsQueue;

    VkPipelineLayout gridPipelineLayout;
    VkPipeline gridPipeline;
    VkBuffer gridVertexBuffer;
    VkDeviceMemory gridVertexBufferMemory;
    uint32_t gridVertexCount;
};
