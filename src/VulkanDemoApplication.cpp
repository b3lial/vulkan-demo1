#include <glm/gtc/matrix_transform.hpp>
#include "Logger.hpp"
#include "VulkanDemoApplication.hpp"
#include "Config.hpp"

//---------------------------------------------------
VulkanDemoApplication::VulkanDemoApplication(WorldCube &worldCube)
    : mWorldCube(worldCube)
{
    // intentionally left blank
}

//---------------------------------------------------
void VulkanDemoApplication::setView(glm::vec3 eye)
{
    view = glm::lookAt(eye,                        // Eye
                       glm::vec3(0.0f),            // Center
                       glm::vec3(0.0f, 1.0f, 0.0f) // Up
    );

    proj = glm::perspective(glm::radians(45.0f), fbWidth / (float)fbHeight, 0.1f,
                            3000.0f);
    proj[1][1] *= -1; // Vulkan Y-Korrektur
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
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    LOG_DEBUG("fb width: " + std::to_string(fbWidth));
    LOG_DEBUG("fb height: " + std::to_string(fbHeight));
    mVulkanGrid.setFramebufferResolution(fbWidth, fbHeight);
}


//---------------------------------------------------
void VulkanDemoApplication::createUniformBuffer()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 uniformBuffer, uniformBufferMemory);
}

//---------------------------------------------------
void VulkanDemoApplication::updateUniformBuffer()
{
    if (lightsSize < 3)
    {
        LOG_DEBUG("add two light sources, otherwise you wont see anything");
        return;
    }

    UniformBufferObject ubo{};
    ubo.lights[0].position = lights[0].position;
    ubo.lights[0].color = lights[0].color;
    ubo.lights[1].position = lights[1].position;
    ubo.lights[1].color = lights[1].color;
    ubo.lights[2].position = lights[2].position;
    ubo.lights[2].color = lights[2].color;

    void *data;
    vkMapMemory(device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, uniformBufferMemory);
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

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                    &descriptorSetLayout) != VK_SUCCESS)
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

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
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
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to allocate descriptor set");
        exit(EXIT_FAILURE);
    }

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

//---------------------------------------------------
void VulkanDemoApplication::initVulkan()
{

    // Create instance
    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t extCount;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extCount);

    VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extCount;
    createInfo.ppEnabledExtensionNames = extensions;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create instance");
        exit(EXIT_FAILURE);
    }

    // Surface
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create window surface");
        exit(EXIT_FAILURE);
    }

    // Physical device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
    physicalDevice = devices[0];
    delete[] devices;
    mVulkanGrid.setPhysicalDevice(physicalDevice);

    // Find queue
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount,
                                             nullptr);
    VkQueueFamilyProperties *queueProps =
        new VkQueueFamilyProperties[queueCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount,
                                             queueProps);
    for (uint32_t i = 0; i < queueCount; ++i)
    {
        VkBool32 supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                             &supported);
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supported)
        {
            queueFamilyIndex = i;
            break;
        }
    }
    delete[] queueProps;

    // Create logical device
    float priority = 1.0f;
    VkDeviceQueueCreateInfo qinfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    qinfo.queueFamilyIndex = queueFamilyIndex;
    qinfo.queueCount = 1;
    qinfo.pQueuePriorities = &priority;

    const char *deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo dinfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    dinfo.queueCreateInfoCount = 1;
    dinfo.pQueueCreateInfos = &qinfo;
    dinfo.enabledExtensionCount = 1;
    dinfo.ppEnabledExtensionNames = deviceExtensions;

    if (vkCreateDevice(physicalDevice, &dinfo, nullptr, &device) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create device");
        exit(EXIT_FAILURE);
    }
    mVulkanGrid.setDevice(device);

    vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);
    mVulkanGrid.setGraphicsQueue(graphicsQueue);

    // Create swapchain
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

    VkSurfaceFormatKHR format;
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                         nullptr);
    VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                         formats);
    format = formats[0];
    delete[] formats;

    VkSwapchainCreateInfoKHR swapInfo{
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapInfo.surface = surface;
    swapInfo.minImageCount = caps.minImageCount;
    swapInfo.imageFormat = format.format;
    swapInfo.imageColorSpace = format.colorSpace;
    if (caps.currentExtent.width != UINT32_MAX)
    {
        swapInfo.imageExtent = caps.currentExtent;
    }
    else
    {
        swapInfo.imageExtent = {static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight)}; // eigene Fenstergröße
    }
    swapInfo.imageArrayLayers = 1;
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapInfo.preTransform = caps.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device, &swapInfo, nullptr, &swapchain) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create swapchain");
        exit(EXIT_FAILURE);
    }

    // Bilder aus der Swapchain holen
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    VkImage *swapchainImages = new VkImage[imageCount];
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages);
    LOG_DEBUG("Swap Chain Images: " + std::to_string(imageCount));

    // Format merken (wird für Renderpass und ImageViews gebraucht)
    swapchainImageFormat = format.format;

    // Image Views erstellen
    swapchainImageViews = new VkImageView[imageCount];
    swapchainImageViewsSize = imageCount;
    for (size_t i = 0; i < imageCount; i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapchainImageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr,
                              &swapchainImageViews[i]) != VK_SUCCESS)
        {
            LOG_DEBUG("Failed to create image views!");
            exit(EXIT_FAILURE);
        }
    }
    delete[] swapchainImages;

    // Now we create the render pass
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat; // Format aus ImageView
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;   // Bild löschen
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Bild speichern
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout =
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Präsentierbereit

    // Attachment Referenz – wird im Subpass benutzt
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Subpass – beschreibt, wie gezeichnet wird
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // Subpass Dependency – Synchronisation (wichtig für Swapchain!)
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Renderpass erstellen
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create render pass!");
        exit(EXIT_FAILURE);
    }

    createDescriptorSetLayout();
    createUniformBuffer();
    createDescriptorPool();
    createDescriptorSet(); // ← Buffer wird hier eingebunden
    updateUniformBuffer(); // ← jetzt kann er korrekt beschrieben werden

    mVulkanGrid.createGridPipeline(renderPass);
    
    // Configure VulkanSpheres object with required parameters
    mVulkanSpheres.setPhysicalDevice(physicalDevice);
    mVulkanSpheres.setDevice(device);
    
    // Create sphere buffers and pipeline
    mVulkanSpheres.createVertexBuffer();
    mVulkanSpheres.createIndexBuffer();
    mVulkanSpheres.createPipeline(device, renderPass, descriptorSetLayout, fbWidth, fbHeight);

    // create framebuffers
    swapchainFramebuffers = new VkFramebuffer[swapchainImageViewsSize];
    swapchainFramebuffersSize = swapchainImageViewsSize;
    for (size_t i = 0; i < swapchainImageViewsSize; i++)
    {
        VkImageView attachments[] = {swapchainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = fbWidth;
        framebufferInfo.height = fbHeight;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                                &swapchainFramebuffers[i]) != VK_SUCCESS)
        {
            LOG_DEBUG("Failed to create framebuffer!");
            exit(EXIT_FAILURE);
        }
    }

    // create command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex =
        queueFamilyIndex; // dieselbe Queue wie beim Zeichnen
    poolInfo.flags = 0;
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create command pool!");
        exit(EXIT_FAILURE);
    }
    mVulkanGrid.setCommandPool(commandPool);

    mVulkanGrid.createGridVertexBuffer();

    // allocate command buffers
    commandBuffers = new VkCommandBuffer[swapchainFramebuffersSize];
    commandBuffersSize = swapchainFramebuffersSize;
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffersSize);
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to allocate command buffers!");
        exit(EXIT_FAILURE);
    }

    LOG_DEBUG("Command Buffers: " + std::to_string(commandBuffersSize));

    // create semaphores
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphore) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create semaphores!");
        exit(EXIT_FAILURE);
    }
}

//---------------------------------------------------
void VulkanDemoApplication::recordCommandBuffer(uint32_t imageIndex, float time)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to begin recording command buffer!");
        exit(EXIT_FAILURE);
    }

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight)};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    // === GRID ZEICHNEN ===
    GridPushConstants gpc{view, proj};

    vkCmdBindPipeline(commandBuffers[imageIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS, mVulkanGrid.getPipeline());

    vkCmdPushConstants(commandBuffers[imageIndex], mVulkanGrid.getPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GridPushConstants),
                       &gpc);

    VkBuffer gridBuffers[] = {mVulkanGrid.getVertexBuffer()};
    VkDeviceSize gridOffsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, gridBuffers,
                           gridOffsets);

    vkCmdDraw(commandBuffers[imageIndex],
              static_cast<uint32_t>(
                  mVulkanGrid.getVertexCount()), // Achtung: zählst du beim Erzeugen
              1, 0, 0);

    // === KUGELN ZEICHNEN ===
    vkCmdBindPipeline(commandBuffers[imageIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS, mVulkanSpheres.getPipeline());

    vkCmdBindDescriptorSets(commandBuffers[imageIndex],
                            VK_PIPELINE_BIND_POINT_GRAPHICS, mVulkanSpheres.getPipelineLayout(), 0,
                            1, &descriptorSet, 0, nullptr);

    VkBuffer vertexBuffers[] = {mVulkanSpheres.getVertexBuffer()}; // dein VkBuffer-Handle
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers,
                           offsets);

    vkCmdBindIndexBuffer(commandBuffers[imageIndex], mVulkanSpheres.getIndexBuffer(), 0,
                         VK_INDEX_TYPE_UINT32);

    // calculate positions of spheres
    unsigned int spheresSize = mWorldCube.getSpheresSize();
    const Sphere *spheres = mWorldCube.getSpheres();
    for (unsigned int j = 0; j < spheresSize; j++)
    {
        Sphere sphere = spheres[j];
        glm::mat4 model =
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(sphere.getPos().x(), sphere.getPos().y(),
                                     sphere.getPos().z())) *
            glm::scale(glm::mat4(1.0f), glm::vec3(sphere.getDiameter()));

        PushConstants pc{model, view, proj};

        vkCmdPushConstants(commandBuffers[imageIndex], mVulkanSpheres.getPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants),
                           &pc);

        vkCmdDrawIndexed(commandBuffers[imageIndex],
                         static_cast<uint32_t>(SPHERE_INDICES), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to record command buffer!");
        exit(EXIT_FAILURE);
    }
}

//---------------------------------------------------
void VulkanDemoApplication::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        float time = static_cast<float>(glfwGetTime());

        // change view every 5 seconds for more dynamic
        if (lastSwitchTime == 0)
        {
            lastSwitchTime = time;
        }
        else if (time - lastSwitchTime > 8)
        {
            lastSwitchTime = time;

            // change view
            orbitRadius =
                rand_float_range(2.5f, 6.0f); // vorher: radiusDist(gen)
            orbitHeight =
                rand_float_range(1.0f, 3.0f); // vorher: heightDist(gen)
            orbitSpeed =
                rand_float_range(0.02f, 0.05f); // vorher: speedDist(gen)

            float axisX =
                rand_float_range(-0.3f, 0.3f); // vorher: axisDist(gen)
            float axisZ =
                rand_float_range(-0.3f, 0.3f); // vorher: axisDist(gen)
            orbitAxis = glm::normalize(glm::vec3(axisX, 1.0f, axisZ));

            if (rand_int_range(0, 1) == 1) // vorher: directionDist(gen)
            {
                orbitSpeed *= -1.0f;
            }
        }

        for (int i = 0; i < 10; i++)
        {
            mWorldCube.stepWorld();
        }
        glfwPollEvents();

        // 1. Bild aus der Swapchain holen
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
                              imageAvailableSemaphore, VK_NULL_HANDLE,
                              &imageIndex);

        // view
        float angle = glm::two_pi<float>() * orbitSpeed * time;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, orbitAxis);
        glm::vec3 eye = glm::vec3(
            rotation * glm::vec4(orbitRadius, orbitHeight, 0.0f, 1.0f));
        setView(eye);

        recordCommandBuffer(imageIndex, time);

        // 2. Infos zum Senden an die Queue
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) !=
            VK_SUCCESS)
        {
            LOG_DEBUG("Failed to submit draw command buffer!");
            exit(EXIT_FAILURE);
        }

        // 3. Bild präsentieren
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(graphicsQueue, &presentInfo);

        vkQueueWaitIdle(graphicsQueue); // synchron für Einfachheit
    }
}

//---------------------------------------------------
void VulkanDemoApplication::cleanup()
{
    LOG_DEBUG("Cleaning up");
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkDestroyBuffer(device, mVulkanSpheres.getIndexBuffer(), nullptr);
    vkFreeMemory(device, mVulkanSpheres.getIndexBufferMemory(), nullptr);
    vkDestroyBuffer(device, mVulkanSpheres.getVertexBuffer(), nullptr);
    vkFreeMemory(device, mVulkanSpheres.getVertexBufferMemory(), nullptr);
    vkDestroyBuffer(device, mVulkanGrid.getVertexBuffer(), nullptr);
    vkFreeMemory(device, mVulkanGrid.getVertexBufferMemory(), nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    delete[] commandBuffers; // since we destroye the command buffers by
                             // destroying their pool, we can now free the array
    for (unsigned int i = 0; i < swapchainFramebuffersSize; i++)
    {
        VkFramebuffer &fb = swapchainFramebuffers[i];
        vkDestroyFramebuffer(device, fb, nullptr);
    }
    delete[] swapchainFramebuffers;
    vkDestroyPipeline(device, mVulkanSpheres.getPipeline(), nullptr);
    vkDestroyPipelineLayout(device, mVulkanSpheres.getPipelineLayout(), nullptr);
    vkDestroyPipeline(device, mVulkanGrid.getPipeline(), nullptr);
    vkDestroyPipelineLayout(device, mVulkanGrid.getPipelineLayout(), nullptr);
    for (unsigned int i = 0; i < swapchainImageViewsSize; i++)
    {
        VkImageView &view = swapchainImageViews[i];
        vkDestroyImageView(device, view, nullptr);
    }
    delete[] swapchainImageViews;
    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}
