#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cstdlib>
#include <string>
#include <vector>

#include "Sphere.hpp"

#pragma once

constexpr uint32_t WIDTH = 1200;
constexpr uint32_t HEIGHT = 1200;

struct GridPushConstants {
    glm::mat4 view;
    glm::mat4 proj;
};

struct PushConstants {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct AnimatedBody {
    glm::vec3 basePosition; // Start-Offset
    float radius;           // Abstand zum Mittelpunkt
    float speed;            // Umdrehungen pro Sekunde
    float phase;            // Startwinkel
};

struct Light {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
};

struct UniformBufferObject {
    Light lights[3];
};

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

    static std::array<VkVertexInputAttributeDescription, 3>
    getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributes{};
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

        return attributes;
    }
};

class VulkanDemoApplication
{
  public:
    void setSpheres(const std::vector<Sphere> &s);
    void setVertices(std::vector<Vertex> &v);
    void setIndices(std::vector<uint32_t> &i);
    void setLights(std::vector<Light> l);
    void setView(glm::vec3 eye);
    void run();

  private:
    void initWindow();
    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);
    void createGraphicsPipeline();
    void createGridPipeline();
    void initVulkan();
    void mainLoop();
    void recordCommandBuffer(uint32_t imageIndex, float time);
    void cleanup();
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);
    void createVertexBuffer();
    void createGridVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();
    void updateUniformBuffer();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSet();
    std::vector<glm::vec3> generateGridLines(int halfExtent = 10, float spacing = 1.0f);

    // vulkan boilerplate init stuff
    GLFWwindow *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    uint32_t queueFamilyIndex;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkFormat swapchainImageFormat;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkPipelineLayout gridPipelineLayout;
    VkPipeline gridPipeline;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    // vertex and index section
    std::vector<Vertex> vertices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer gridVertexBuffer;
    VkDeviceMemory gridVertexBufferMemory;
    uint32_t gridVertexCount;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    std::vector<uint32_t> indices;

    // uniform buffer
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

    // lights for the scenery
    std::vector<Light> lights;

    // spheres we want to display
    std::vector<Sphere> spheres;

    // camera matrix
    glm::mat4 view;
    glm::mat4 proj;

    // camera orbit
    float orbitRadius = 3.0f;
    float orbitHeight = 2.0f;     // Y-Position bleibt konstant
    float orbitSpeed = 0.05f;      // Umdrehungen pro Sekunde
};
