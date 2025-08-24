#include "VulkanCube.hpp"

VulkanCube::VulkanCube() : mVerticesSize(0), mIndicesSize(0)
{
    // Initialize the static indices for cube faces
    // Each face uses 2 triangles (6 indices)
    for (int face = 0; face < 6; ++face) {
        int baseVertex = face * 4;
        int baseIndex = face * 6;
        
        // First triangle (0, 1, 2)
        mIndices[baseIndex + 0] = baseVertex + 0;
        mIndices[baseIndex + 1] = baseVertex + 1;
        mIndices[baseIndex + 2] = baseVertex + 2;
        
        // Second triangle (0, 2, 3)
        mIndices[baseIndex + 3] = baseVertex + 0;
        mIndices[baseIndex + 4] = baseVertex + 2;
        mIndices[baseIndex + 5] = baseVertex + 3;
    }
    mIndicesSize = 36; // 6 faces * 6 indices per face
}

void VulkanCube::generateVerticesFromSides(const WorldCube::Side* sides, double edgeLength)
{
    mVerticesSize = 0;
    
    for (int i = 0; i < 6; ++i) {
        glm::dvec3 corners[4];
        sides[i].getCorners(corners, edgeLength);
        
        // Convert corners to vertices and add them
        for (int j = 0; j < 4; ++j) {
            mVertices[mVerticesSize].pos = glm::vec3(corners[j]);
            mVertices[mVerticesSize].color = glm::vec3(0.5f, 0.5f, 1.0f); // Light blue for transparency effect
            mVertices[mVerticesSize].normal = glm::vec3(sides[i].normal());
            mVerticesSize++;
        }
    }
}

void VulkanCube::createVertexBuffer()
{

}

void VulkanCube::createIndexBuffer()
{

}

void VulkanCube::createPipeline(VkRenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout, int fbWidth, int fbHeight)
{

}

void VulkanCube::draw(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const WorldCube::Side* sides, double edgeLength)
{

}
