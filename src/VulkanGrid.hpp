#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Config.hpp"
#include "VulkanBase.hpp"

struct GridPushConstants
{
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanGrid : public VulkanBase
{
  public:
    void createPipeline(VkRenderPass &renderPass);
    void createVertexBuffer();
    void draw(VkCommandBuffer commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

    // Setters
    void setFramebufferResolution(int fbWidth, int fbHeight){ mFbWidth = fbWidth; mFbHeight = fbHeight; }

  private:
    int generateLines(int halfExtent, float spacing, glm::vec3 lines[]);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  
    int mFbWidth = WIDTH;
    int mFbHeight = HEIGHT;

    uint32_t mVertexCount;
};
