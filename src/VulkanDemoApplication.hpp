#pragma once

#include "ShaderData.hpp"
#include "VulkanSpheres.hpp"
#include "WorldCube.hpp"
#include "VulkanGrid.hpp"
#include "VulkanCamera.hpp"

class VulkanDemoApplication
{
  public:
    VulkanDemoApplication(WorldCube &worldCube);

    void setLights(Light l[], int size)
    {
        mVulkanCamera.setLights(l, size);
    }

    void setView(glm::vec3 eye);
    void run();

  private:
    // framebuffer size, can differ from actual window size in pixels
    int fbWidth = 0, fbHeight = 0;

    WorldCube &mWorldCube;
    VulkanSpheres mVulkanSpheres;
    VulkanGrid mVulkanGrid;
    VulkanCamera mVulkanCamera;

    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    void recordCommandBuffer(uint32_t imageIndex, float time);
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
    VkFramebuffer *swapchainFramebuffers;
    unsigned int swapchainFramebuffersSize;
    VkCommandPool commandPool;
    VkCommandBuffer *commandBuffers;
    unsigned int commandBuffersSize;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    // uniform buffer
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    // descriptor
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
};
