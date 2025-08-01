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
    GLFWwindow *mWindow;
    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
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
