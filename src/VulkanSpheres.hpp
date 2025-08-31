#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "ShaderData.hpp"
#include "Config.hpp"
#include "WorldSphere.hpp"
#include "VulkanBase.hpp"

struct SpheresPushConstants
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanSpheres : public VulkanBase
{
public:
    VulkanSpheres(float radius = 0.5f, int sectors = SPHERE_VERTICE_SECTORS, int stacks = SPHERE_VERTICE_STACKS);
    void createVertexBuffer();
    void createIndexBuffer();
    void createPipeline(VkRenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout);
    void draw(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const WorldSphere* spheres, unsigned int spheresSize);
    
    // Getters
    VkBuffer& getIndexBuffer() { return mIndexBuffer; }
    VkDeviceMemory& getIndexBufferMemory() { return mIndexBufferMemory; }

private:
    int generateVertices(float radius, int sectors, int stacks);
    int generateIndices(int sectors, int stacks);
    
    Vertex mVertices[SPHERE_VERTICES];
    int mVerticesSize;
    uint32_t mIndices[SPHERE_INDICES];
    int mIndicesSize;
    
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
};