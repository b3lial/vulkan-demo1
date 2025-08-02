#pragma once

#include "VulkanSpheres.hpp"
#include "WorldCube.hpp"
#include "VulkanGrid.hpp"
#include "VulkanCamera.hpp"

class VulkanDemoApplication
{
  public:
    VulkanDemoApplication(WorldCube &worldCube);

    void run();
    
    // Access to camera
    VulkanCamera& getCamera() { return mVulkanCamera; }

  private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    /// Creates the Vulkan instance with required GLFW extensions
    void createInstance();
    /// Finds a queue family that supports both graphics operations and presentation
    void findQueueFamily();
    /// Creates the logical device with swapchain extension and graphics queue
    void createLogicalDevice();
    /// Creates the swapchain for double/triple buffering presentation
    void createSwapchain();
    /// Creates image views for swapchain images to access them in shaders
    void createImageViews();
    /// Creates the render pass defining rendering operations and attachments
    void createRenderPass();

    void recordCommandBuffer(uint32_t imageIndex, float time);
    void createUniformBuffer();
    void updateUniformBuffer();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSet();

    // framebuffer size, can differ from actual window size in pixels
    int mFbWidth = 0, mFbHeight = 0;

    WorldCube &mWorldCube;
    VulkanSpheres mVulkanSpheres;
    VulkanGrid mVulkanGrid;
    VulkanCamera mVulkanCamera;

    // vulkan boilerplate init stuff
    GLFWwindow *mWindow;
    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mLogicalDevice;
    VkQueue mGraphicsQueue;
    uint32_t mQueueFamilyIndex;
    VkSwapchainKHR mSwapchain;
    VkImageView *mSwapchainImageViews;
    unsigned int mSwapchainImageViewsSize;
    VkFormat mSwapchainImageFormat;
    VkRenderPass mRenderPass;
    VkFramebuffer *mSwapchainFramebuffers;
    unsigned int mSwapchainFramebuffersSize;
    VkCommandPool mCommandPool;
    VkCommandBuffer *mCommandBuffers;
    unsigned int mCommandBuffersSize;
    VkSemaphore mImageAvailableSemaphore;
    VkSemaphore mRenderFinishedSemaphore;

    // uniform buffer
    VkBuffer mUniformBuffer;
    VkDeviceMemory mUniformBufferMemory;

    // descriptor
    VkDescriptorSetLayout mDescriptorSetLayout;
    VkDescriptorPool mDescriptorPool;
    VkDescriptorSet mDescriptorSet;
};
