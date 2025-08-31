#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ShaderData.hpp"
#include "WorldCube.hpp"
#include "VulkanBase.hpp"

struct CubePushConstants
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanCube : public VulkanBase
{
public:
    VulkanCube();
    void createVertexBuffer(const WorldCube::Side* sides, double edgeLength);
    void createIndexBuffer();
    void createPipeline(VkRenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout, int fbWidth, int fbHeight);
    void draw(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const WorldCube::Side* sides, double edgeLength);
    
    // Getters
    VkBuffer& getIndexBuffer() { return mIndexBuffer; }
    VkDeviceMemory& getIndexBufferMemory() { return mIndexBufferMemory; }

private:
    void generateVerticesFromSides(const WorldCube::Side* sides, double edgeLength);
    
    static constexpr int MAX_VERTICES = 24;  // 6 sides * 4 vertices each
    static constexpr int MAX_INDICES = 36;   // 6 sides * 6 indices each
    
    Vertex mVertices[MAX_VERTICES];
    int mVerticesSize;
    uint32_t mIndices[MAX_INDICES];
    int mIndicesSize;
    
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
};