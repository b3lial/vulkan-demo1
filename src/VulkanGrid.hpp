#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class VulkanGrid
{
  public:
    void createGridPipeline(VkDevice &device, VkRenderPass &renderPass);
    void createGridVertexBuffer(VkDevice &device);

  private:
    int generateGridLines(int halfExtent, float spacing, glm::vec3 lines[]);

    VkPipelineLayout gridPipelineLayout;
    VkPipeline gridPipeline;
    VkBuffer gridVertexBuffer;
    VkDeviceMemory gridVertexBufferMemory;
    uint32_t gridVertexCount;
};
