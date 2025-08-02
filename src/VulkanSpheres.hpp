#pragma once

#include "ShaderData.hpp"
#include "Config.hpp"

class VulkanSpheres
{
public:
    VulkanSpheres(float radius = 0.5f, int sectors = SPHERE_VERTICE_SECTORS, int stacks = SPHERE_VERTICE_STACKS);
    void createVertexBuffer();
    void createIndexBuffer();
    void createPipeline(VkRenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout, int fbWidth, int fbHeight);
    
    // Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice){ mPhysicalDevice = physicalDevice; }
    void setLogicalDevice(VkDevice &device){ mLogicalDevice = device; }
    
    // Getters
    VkBuffer& getVertexBuffer() { return mVertexBuffer; }
    VkBuffer& getIndexBuffer() { return mIndexBuffer; }
    VkDeviceMemory& getVertexBufferMemory() { return mVertexBufferMemory; }
    VkDeviceMemory& getIndexBufferMemory() { return mIndexBufferMemory; }
    VkPipeline& getPipeline() { return mPipeline; }
    VkPipelineLayout& getPipelineLayout() { return mPipelineLayout; }

private:
    int generateVertices(float radius, int sectors, int stacks);
    int generateIndices(int sectors, int stacks);

    VkPhysicalDevice mPhysicalDevice;
    VkDevice mLogicalDevice;
    
    Vertex mVertices[SPHERE_VERTICES];
    int mVerticesSize;
    uint32_t mIndices[SPHERE_INDICES];
    int mIndicesSize;
    
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;

    VkPipeline mPipeline;
    VkPipelineLayout mPipelineLayout;
};