#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cstdlib>
#include <string>
#include <vector>

#pragma once

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 800;

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
    Light lights[2];
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
    void setVertices(std::vector<Vertex> &v);
    void setIndices(std::vector<uint32_t> &i);
    void setLights(std::vector<Light> l);
    void run();

  private:
    void initWindow();
    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);
    void createGraphicsPipeline();
    void initVulkan();
    void mainLoop();
    void recordCommandBuffer(uint32_t imageIndex, float time);
    void cleanup();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);
    void createVertexBuffer();
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
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkFormat swapchainImageFormat;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    // vertex and index section
    std::vector<Vertex> vertices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

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
    std::vector<AnimatedBody> animatedBodies = {
        {{0.0f, 0.0f, 0.0f}, 0.5f, 0.2f, 0},
        {{-1.0f, 0.0f, -0.4f}, 0.5f, 0.3f, 0},
        {{ 1.0f, 0.0f, -0.3f}, 0.3f, 0.15f, 0}
    };
};
