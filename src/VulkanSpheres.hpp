#pragma once

#include "ShaderData.hpp"

class VulkanSpheres
{
public:
    void createVertexBuffer();
    void createIndexBuffer();
    
    // Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice)
    {
        mPhysicalDevice = physicalDevice;
    }
    
    void setDevice(VkDevice &device)
    {
        mDevice = device;
    }
    
    void setVertices(Vertex *vertices, int verticesSize)
    {
        mVertices = vertices;
        mVerticesSize = verticesSize;
    }
    
    void setIndices(uint32_t *indices, int indicesSize)
    {
        mIndices = indices;
        mIndicesSize = indicesSize;
    }
    
    // Getters
    VkBuffer& getVertexBuffer() { return mVertexBuffer; }
    VkBuffer& getIndexBuffer() { return mIndexBuffer; }
    VkDeviceMemory& getVertexBufferMemory() { return mVertexBufferMemory; }
    VkDeviceMemory& getIndexBufferMemory() { return mIndexBufferMemory; }

private:
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    
    Vertex *mVertices;
    int mVerticesSize;
    uint32_t *mIndices;
    int mIndicesSize;
    
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
};

int generateSphereVertices(float radius, int sectors, int stacks,
                           Vertex vertices[]);
int generateSphereIndices(int sectors, int stacks, uint32_t indices[]);