#include <glm/gtc/matrix_transform.hpp>
#include "Logger.hpp"
#include "VulkanDemoApplication.hpp"
#include "Config.hpp"

#include <memory.h>

//---------------------------------------------------
VulkanDemoApplication::VulkanDemoApplication(WorldCube &worldCube)
    : mWorldCube(worldCube)
{
    // intentionally left blank
}

//---------------------------------------------------
void VulkanDemoApplication::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

//---------------------------------------------------
void VulkanDemoApplication::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    mWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwGetFramebufferSize(mWindow, &mFbWidth, &mFbHeight);
    LOG_DEBUG("fb width: " + std::to_string(mFbWidth));
    LOG_DEBUG("fb height: " + std::to_string(mFbHeight));
    mVulkanGrid.setFramebufferResolution(mFbWidth, mFbHeight);
}


//---------------------------------------------------
void VulkanDemoApplication::createUniformBuffer()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    createBuffer(mPhysicalDevice, mLogicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 mUniformBuffer, mUniformBufferMemory);
}

//---------------------------------------------------
void VulkanDemoApplication::updateUniformBuffer()
{
    // Camera always has exactly LIGHTS_AMOUNT lights

    Light* lights = mVulkanCamera.getLights();
    UniformBufferObject ubo{};
    ubo.lights[0].position = lights[0].position;
    ubo.lights[0].color = lights[0].color;
    ubo.lights[1].position = lights[1].position;
    ubo.lights[1].color = lights[1].color;
    ubo.lights[2].position = lights[2].position;
    ubo.lights[2].color = lights[2].color;

    void *data;
    vkMapMemory(mLogicalDevice, mUniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(mLogicalDevice, mUniformBufferMemory);
}

//---------------------------------------------------
void VulkanDemoApplication::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(mLogicalDevice, &layoutInfo, nullptr,
                                    &mDescriptorSetLayout) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create descriptor set layout");
        exit(EXIT_FAILURE);
    }
}

//---------------------------------------------------
void VulkanDemoApplication::createDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(mLogicalDevice, &poolInfo, nullptr, &mDescriptorPool) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create descriptor pool");
        exit(EXIT_FAILURE);
    }
}

//---------------------------------------------------
void VulkanDemoApplication::createDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &mDescriptorSetLayout;

    if (vkAllocateDescriptorSets(mLogicalDevice, &allocInfo, &mDescriptorSet) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to allocate descriptor set");
        exit(EXIT_FAILURE);
    }

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = mUniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = mDescriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(mLogicalDevice, 1, &descriptorWrite, 0, nullptr);
}

//---------------------------------------------------
void VulkanDemoApplication::createInstance()
{
    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "Vulkan Demo 1";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t extCount;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extCount);

    VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extCount;
    createInfo.ppEnabledExtensionNames = extensions;

    if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create instance");
        exit(EXIT_FAILURE);
    }
}

//---------------------------------------------------
void VulkanDemoApplication::findQueueFamily()
{
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount,
                                             nullptr);
    LOG_DEBUG("Available Queue Families: " + std::to_string(queueCount));
    VkQueueFamilyProperties *queueProps =
        new VkQueueFamilyProperties[queueCount];
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount,
                                             queueProps);
    for (uint32_t i = 0; i < queueCount; ++i)
    {
        LOG_DEBUG("Queue Family " + std::to_string(i) + ": " + std::to_string(queueProps[i].queueCount) + " queues");
        VkBool32 supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface,
                                             &supported);
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supported)
        {
            mQueueFamilyIndex = i;
            break;
        }
    }
    delete[] queueProps;
    LOG_DEBUG("Selected Queue: " + std::to_string(mQueueFamilyIndex));
}

//---------------------------------------------------
void VulkanDemoApplication::createLogicalDevice()
{
    float priority = 1.0f;
    VkDeviceQueueCreateInfo qinfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    qinfo.queueFamilyIndex = mQueueFamilyIndex;
    qinfo.queueCount = 1;
    qinfo.pQueuePriorities = &priority;

    const char *deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo dinfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    dinfo.queueCreateInfoCount = 1;
    dinfo.pQueueCreateInfos = &qinfo;
    dinfo.enabledExtensionCount = 1;
    dinfo.ppEnabledExtensionNames = deviceExtensions;

    if (vkCreateDevice(mPhysicalDevice, &dinfo, nullptr, &mLogicalDevice) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create device");
        exit(EXIT_FAILURE);
    }
    mVulkanGrid.setLogicalDevice(mLogicalDevice);
}

//---------------------------------------------------
void VulkanDemoApplication::createSwapchain()
{
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &caps);

    VkSurfaceFormatKHR format;
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount,
                                         nullptr);
    VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount,
                                         formats);
    format = formats[0];
    delete[] formats;

    VkSwapchainCreateInfoKHR swapInfo{
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapInfo.surface = mSurface;
    swapInfo.minImageCount = caps.minImageCount;
    swapInfo.imageFormat = format.format;
    swapInfo.imageColorSpace = format.colorSpace;
    if (caps.currentExtent.width != UINT32_MAX)
    {
        swapInfo.imageExtent = caps.currentExtent;
    }
    else
    {
        swapInfo.imageExtent = {static_cast<uint32_t>(mFbWidth), static_cast<uint32_t>(mFbHeight)}; // eigene Fenstergröße
    }
    swapInfo.imageArrayLayers = 1;
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapInfo.preTransform = caps.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(mLogicalDevice, &swapInfo, nullptr, &mSwapchain) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create swapchain");
        exit(EXIT_FAILURE);
    }

    mSwapchainImageFormat = format.format;
}

//---------------------------------------------------
void VulkanDemoApplication::createImageViews()
{
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(mLogicalDevice, mSwapchain, &imageCount, nullptr);
    VkImage *swapchainImages = new VkImage[imageCount];
    vkGetSwapchainImagesKHR(mLogicalDevice, mSwapchain, &imageCount, swapchainImages);
    LOG_DEBUG("Swap Chain Images: " + std::to_string(imageCount));

    mSwapchainImageViews = new VkImageView[imageCount];
    mSwapchainImageViewsSize = imageCount;
    for (size_t i = 0; i < imageCount; i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = mSwapchainImageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(mLogicalDevice, &viewInfo, nullptr,
                              &mSwapchainImageViews[i]) != VK_SUCCESS)
        {
            LOG_DEBUG("Failed to create image views!");
            exit(EXIT_FAILURE);
        }
    }
    delete[] swapchainImages;
}

//---------------------------------------------------
void VulkanDemoApplication::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mSwapchainImageFormat; // Format aus ImageView
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;   // Bild löschen
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Bild speichern
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout =
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Präsentierbereit

    // Attachment Reference - links attachment description to subpass
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Subpass - describes the actual rendering operation within the render pass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // Subpass Dependency - synchronization with swapchain (ensures proper timing)
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Create the render pass object
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(mLogicalDevice, &renderPassInfo, nullptr, &mRenderPass) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create render pass!");
        exit(EXIT_FAILURE);
    }
}

//---------------------------------------------------
void VulkanDemoApplication::createFramebuffers()
{
    mSwapchainFramebuffers = new VkFramebuffer[mSwapchainImageViewsSize];
    mSwapchainFramebuffersSize = mSwapchainImageViewsSize;
    for (size_t i = 0; i < mSwapchainImageViewsSize; i++)
    {
        VkImageView attachments[] = {mSwapchainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = mFbWidth;
        framebufferInfo.height = mFbHeight;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(mLogicalDevice, &framebufferInfo, nullptr,
                                &mSwapchainFramebuffers[i]) != VK_SUCCESS)
        {
            LOG_DEBUG("Failed to create framebuffer!");
            exit(EXIT_FAILURE);
        }
    }
}

//---------------------------------------------------
void VulkanDemoApplication::initVulkan()
{
    createInstance();

    // Surface
    if (glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create window surface");
        exit(EXIT_FAILURE);
    }

    // Physical device. Select the first one available
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
    VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices);
    mPhysicalDevice = devices[0];
    delete[] devices;
    mVulkanGrid.setPhysicalDevice(mPhysicalDevice);

    findQueueFamily();
    createLogicalDevice();

    // Get first queue of the previously selected queue family
    // Queue families are categories of queues with same capabilities (graphics, compute, transfer).
    // We found a family that supports both graphics operations and presentation to screen.
    // This retrieves the actual queue handle to submit rendering commands to.
    vkGetDeviceQueue(mLogicalDevice, mQueueFamilyIndex, 0, &mGraphicsQueue);

    createSwapchain();
    createImageViews();
    createRenderPass();
    createFramebuffers();

    createDescriptorSetLayout();
    createUniformBuffer();
    createDescriptorPool();
    createDescriptorSet();
    updateUniformBuffer();

    // create command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex =
        mQueueFamilyIndex; // dieselbe Queue wie beim Zeichnen
    poolInfo.flags = 0;
    if (vkCreateCommandPool(mLogicalDevice, &poolInfo, nullptr, &mCommandPool) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create command pool!");
        exit(EXIT_FAILURE);
    }

    // allocate command buffers
    mCommandBuffers = new VkCommandBuffer[mSwapchainFramebuffersSize];
    mCommandBuffersSize = mSwapchainFramebuffersSize;
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffersSize);
    if (vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, mCommandBuffers) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to allocate command buffers!");
        exit(EXIT_FAILURE);
    }

    LOG_DEBUG("Command Buffers: " + std::to_string(mCommandBuffersSize));

    // create semaphores
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr,
                          &mImageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr,
                          &mRenderFinishedSemaphore) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create semaphores!");
        exit(EXIT_FAILURE);
    }

    // init grid 
    mVulkanGrid.setGraphicsQueue(mGraphicsQueue);
    mVulkanGrid.setCommandPool(mCommandPool);
    mVulkanGrid.createVertexBuffer();
    mVulkanGrid.createPipeline(mRenderPass);
    
    // init spheres
    mVulkanSpheres.setPhysicalDevice(mPhysicalDevice);
    mVulkanSpheres.setLogicalDevice(mLogicalDevice);
    mVulkanSpheres.createVertexBuffer();
    mVulkanSpheres.createIndexBuffer();
    mVulkanSpheres.createPipeline(mRenderPass, mDescriptorSetLayout, mFbWidth, mFbHeight);

    // init cube
    mVulkanCube.setPhysicalDevice(mPhysicalDevice);
    mVulkanCube.setLogicalDevice(mLogicalDevice);
    mVulkanCube.createVertexBuffer(mWorldCube.getSides(), mWorldCube.getEdgeLength());
    mVulkanCube.createIndexBuffer();
    mVulkanCube.createPipeline(mRenderPass, mDescriptorSetLayout, mFbWidth, mFbHeight);
}

//---------------------------------------------------
void VulkanDemoApplication::recordCommandBuffer(uint32_t imageIndex, float time)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(mCommandBuffers[imageIndex], &beginInfo) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to begin recording command buffer!");
        exit(EXIT_FAILURE);
    }

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.framebuffer = mSwapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {static_cast<uint32_t>(mFbWidth), static_cast<uint32_t>(mFbHeight)};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(mCommandBuffers[imageIndex], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    // === Draw Cube ===
    mVulkanCube.draw(mCommandBuffers[imageIndex], mDescriptorSet, mVulkanCamera.getViewMatrix(), mVulkanCamera.getProjectionMatrix(), 
        mWorldCube.getSides(), mWorldCube.getEdgeLength()
    );

    // === Draw Grid ===
    mVulkanGrid.draw(mCommandBuffers[imageIndex], mVulkanCamera.getViewMatrix(), mVulkanCamera.getProjectionMatrix());

    // === Draw Spheres ===
    mVulkanSpheres.draw(mCommandBuffers[imageIndex], mDescriptorSet, 
        mVulkanCamera.getViewMatrix(), mVulkanCamera.getProjectionMatrix(), 
        mWorldCube.getSpheres(), mWorldCube.getSpheresSize()
    );

    vkCmdEndRenderPass(mCommandBuffers[imageIndex]);

    if (vkEndCommandBuffer(mCommandBuffers[imageIndex]) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to record command buffer!");
        exit(EXIT_FAILURE);
    }
}

//---------------------------------------------------
void VulkanDemoApplication::mainLoop()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        float time = static_cast<float>(glfwGetTime());

        // Update camera animation
        mVulkanCamera.updateCamera(time, mFbWidth, mFbHeight);

        for (int i = 0; i < 10; i++)
        {
            mWorldCube.stepWorld();
        }
        glfwPollEvents();

        // 1. Fetch Image from Swapchain
        uint32_t imageIndex;
        vkAcquireNextImageKHR(mLogicalDevice, mSwapchain, UINT64_MAX,
                              mImageAvailableSemaphore, VK_NULL_HANDLE,
                              &imageIndex);

        // Here is the real work done. We draw Grid and Spheres
        recordCommandBuffer(imageIndex, time);

        // 2. Infos for sending to the queue
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {mImageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) !=
            VK_SUCCESS)
        {
            LOG_DEBUG("Failed to submit draw command buffer!");
            exit(EXIT_FAILURE);
        }

        // 3. Present Image
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &mSwapchain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(mGraphicsQueue, &presentInfo);

        vkQueueWaitIdle(mGraphicsQueue); // synchron für Einfachheit
    }
}

//---------------------------------------------------
void VulkanDemoApplication::cleanup()
{
    LOG_DEBUG("Cleaning up");
    vkDestroyDescriptorPool(mLogicalDevice, mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mLogicalDevice, mDescriptorSetLayout, nullptr);
    vkDestroyBuffer(mLogicalDevice, mVulkanSpheres.getIndexBuffer(), nullptr);
    vkFreeMemory(mLogicalDevice, mVulkanSpheres.getIndexBufferMemory(), nullptr);
    vkDestroyBuffer(mLogicalDevice, mVulkanSpheres.getVertexBuffer(), nullptr);
    vkFreeMemory(mLogicalDevice, mVulkanSpheres.getVertexBufferMemory(), nullptr);
    vkDestroyBuffer(mLogicalDevice, mVulkanGrid.getVertexBuffer(), nullptr);
    vkFreeMemory(mLogicalDevice, mVulkanGrid.getVertexBufferMemory(), nullptr);
    vkDestroySemaphore(mLogicalDevice, mRenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(mLogicalDevice, mImageAvailableSemaphore, nullptr);
    vkDestroyCommandPool(mLogicalDevice, mCommandPool, nullptr);
    delete[] mCommandBuffers; // since we destroye the command buffers by
                             // destroying their pool, we can now free the array
    for (unsigned int i = 0; i < mSwapchainFramebuffersSize; i++)
    {
        VkFramebuffer &fb = mSwapchainFramebuffers[i];
        vkDestroyFramebuffer(mLogicalDevice, fb, nullptr);
    }
    delete[] mSwapchainFramebuffers;
    vkDestroyPipeline(mLogicalDevice, mVulkanSpheres.getPipeline(), nullptr);
    vkDestroyPipelineLayout(mLogicalDevice, mVulkanSpheres.getPipelineLayout(), nullptr);
    vkDestroyPipeline(mLogicalDevice, mVulkanGrid.getPipeline(), nullptr);
    vkDestroyPipelineLayout(mLogicalDevice, mVulkanGrid.getPipelineLayout(), nullptr);
    for (unsigned int i = 0; i < mSwapchainImageViewsSize; i++)
    {
        VkImageView &view = mSwapchainImageViews[i];
        vkDestroyImageView(mLogicalDevice, view, nullptr);
    }
    delete[] mSwapchainImageViews;
    vkDestroyRenderPass(mLogicalDevice, mRenderPass, nullptr);
    vkDestroySwapchainKHR(mLogicalDevice, mSwapchain, nullptr);
    vkDestroyDevice(mLogicalDevice, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyInstance(mInstance, nullptr);
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}
