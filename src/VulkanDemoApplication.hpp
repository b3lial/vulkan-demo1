#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "WorldCube.hpp"

#pragma once

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 800;

constexpr uint32_t SPHERE_VERTICE_SECTORS = 48;
constexpr uint32_t SPHERE_VERTICE_STACKS = 32;
constexpr uint32_t SPHERE_VERTICES =
    (SPHERE_VERTICE_SECTORS + 1) * (SPHERE_VERTICE_STACKS + 1);
constexpr uint32_t SPHERE_INDICES =
    SPHERE_VERTICE_SECTORS * SPHERE_VERTICE_STACKS * 6;

constexpr uint32_t LIGHTS_AMOUNT = 3;

constexpr uint32_t GRID_HALF_EXTEND = 10;
constexpr uint32_t GRID_VERTEX_COUNT = (GRID_HALF_EXTEND * 2 + 1) * 4;

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
    VulkanDemoApplication(WorldCube &worldCube);

    void setVertices(Vertex v[], int size)
    {
        vertices = v;
        verticesSize = size;
    }

    void setIndices(uint32_t i[], int size)
    {
        indices = i;
        indicesSize = size;
    }

    void setLights(Light l[], int size)
    {
        lights = l;
        lightsSize = size;
    }

    void setView(glm::vec3 eye);
    void run();

  private:
    WorldCube &mWorldCube;

    VkShaderModule createShaderModule(const char *code, size_t size);
    char *readFile(const char *filename, size_t *size);

    void initWindow();
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
    int generateGridLines(int halfExtent, float spacing, glm::vec3 lines[]);

    // vulkan boilerplate init stuff
    GLFWwindow *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    uint32_t queueFamilyIndex;
    VkSwapchainKHR swapchain;
    VkImageView *swapchainImageViews;
    unsigned int swapchainImageViewsSize;
    VkFormat swapchainImageFormat;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkPipelineLayout gridPipelineLayout;
    VkPipeline gridPipeline;
    VkFramebuffer *swapchainFramebuffers;
    unsigned int swapchainFramebuffersSize;
    VkCommandPool commandPool;
    VkCommandBuffer *commandBuffers;
    unsigned int commandBuffersSize;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    // vertex and indices section
    Vertex *vertices;
    int verticesSize;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer gridVertexBuffer;
    VkDeviceMemory gridVertexBufferMemory;
    uint32_t gridVertexCount;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    uint32_t *indices;
    int indicesSize;

    // uniform buffer
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

    // lights for the scenery
    Light *lights;
    int lightsSize;

    // camera matrix
    glm::mat4 view;
    glm::mat4 proj;

    // camera orbit
    float orbitRadius = 3.0f;
    float orbitHeight = 2.0f; // Y-Position bleibt konstant
    float orbitSpeed = 0.05f; // Umdrehungen pro Sekunde
    glm::vec3 orbitAxis = glm::normalize(glm::vec3(0.1, 1.0f, 0.1));

    // animation
    float lastSwitchTime = 0;
    uint32_t rng_state = 0xDEADBEEF;
    uint32_t rand_u32()
    {
        rng_state = rng_state * 1664525 + 1013904223;
        return rng_state;
    }
    float rand_float_range(float min, float max)
    {
        return min + (rand_u32() / (float)UINT32_MAX) * (max - min);
    }
    int rand_int_range(int min, int max)
    {
        return min + (rand_u32() % (max - min + 1));
    }
};
