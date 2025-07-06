#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct GridPushConstants
{
    glm::mat4 view;
    glm::mat4 proj;
};

struct PushConstants
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct Light
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
};

struct UniformBufferObject
{
    Light lights[3];
};

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
        attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT; // normal
        attributes[2].offset = offsetof(Vertex, normal);
    }
};

VkShaderModule createShaderModule(VkDevice &device, const char *code, size_t size);
char *readFile(const char *filename, size_t *size);
void createBuffer(VkPhysicalDevice &physicalDevice, VkDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
