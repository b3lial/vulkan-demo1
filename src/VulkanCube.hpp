#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class VulkanCube
{
public:
    void createVertexBuffer();
    void createIndexBuffer();
    void createPipeline(VkRenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout, int fbWidth, int fbHeight);
    void draw(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, const glm::mat4& viewMatrix, const glm::mat4& projMatrix);
};