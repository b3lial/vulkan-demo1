#pragma once

#include "ShaderData.hpp"
#include "VulkanSpheres.hpp"
#include "WorldCube.hpp"
#include "VulkanGrid.hpp"

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
    // framebuffer size, can differ from actual window size in pixels
    int fbWidth = 0, fbHeight = 0;

    WorldCube &mWorldCube;
    VulkanSpheres mVulkanSpheres;
    VulkanGrid mVulkanGrid;

    void initWindow();
    void createSpheresPipeline();
    
    void initVulkan();
    void mainLoop();
    void recordCommandBuffer(uint32_t imageIndex, float time);
    void cleanup();
    void createSpheresVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();
    void updateUniformBuffer();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSet();

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
