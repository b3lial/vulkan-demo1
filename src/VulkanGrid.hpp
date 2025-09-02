#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "VulkanBase.hpp"

constexpr uint32_t GRID_HALF_EXTEND = 10;
constexpr uint32_t GRID_VERTEX_COUNT = (GRID_HALF_EXTEND * 2 + 1) * 4;

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

  private:
    int generateLines(int halfExtent, float spacing, glm::vec3 lines[]);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  
    uint32_t mVertexCount;
};
