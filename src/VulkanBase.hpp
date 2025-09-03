#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

constexpr uint32_t ATTRIBUTES_DESCRIPTION_SIZE = 3;

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
    }

    static void getAttributeDescriptions(VkVertexInputAttributeDescription* attributes)
    {
        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].offset = offsetof(Vertex, pos);

        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset = offsetof(Vertex, color);

        attributes[2].binding = 0;
        attributes[2].location = 2;
        attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[2].offset = offsetof(Vertex, normal);
    }
};

class VulkanBase
{
public:
    // Setters
    void setPhysicalDevice(VkPhysicalDevice &physicalDevice) { mPhysicalDevice = physicalDevice; }
    void setLogicalDevice(VkDevice &device) { mLogicalDevice = device; }
    void setCommandPool(VkCommandPool &commandPool) { mCommandPool = commandPool; }
    void setGraphicsQueue(VkQueue graphicsQueue) { mGraphicsQueue = graphicsQueue; }
    void setFramebufferResolution(int fbWidth, int fbHeight) { mFbWidth = fbWidth; mFbHeight = fbHeight; }

    // Static utility functions
    static VkShaderModule createShaderModule(VkDevice &device, const char *code, size_t size);
    static void createBuffer(VkPhysicalDevice &physicalDevice, VkDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    static uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    // Getters
    VkBuffer& getVertexBuffer() { return mVertexBuffer; }
    VkDeviceMemory& getVertexBufferMemory() { return mVertexBufferMemory; }
    VkBuffer& getIndexBuffer() { return mIndexBuffer; }
    VkDeviceMemory& getIndexBufferMemory() { return mIndexBufferMemory; }
    VkPipeline& getPipeline() { return mPipeline; }
    VkPipelineLayout& getPipelineLayout() { return mPipelineLayout; }

protected:
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mLogicalDevice;
    VkCommandPool mCommandPool;
    VkQueue mGraphicsQueue;
    
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    VkPipeline mPipeline;
    VkPipelineLayout mPipelineLayout;
    
    int mFbWidth = -1;
    int mFbHeight = -1;
};