#include "VulkanSpheres.hpp"
#include "Logger.hpp"
#include "EmbeddedShaders.hpp"

#include <math.h>
#include <memory.h>

VulkanSpheres::VulkanSpheres(float radius, int sectors, int stacks)
{
    mVerticesSize = generateVertices(radius, sectors, stacks);
    mIndicesSize = generateIndices(sectors, stacks);
}

int VulkanSpheres::generateVertices(float radius, int sectors, int stacks)
{
    const float PI = 3.14159265359f;
    int index = 0;

    for (int i = 0; i <= stacks; ++i)
    {
        float stackAngle = PI / 2 - i * PI / stacks; // von +π/2 bis -π/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j)
        {
            float sectorAngle = j * 2 * PI / sectors;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            glm::vec3 position = glm::vec3(x, y, z);
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

            mVertices[index] = {position, color, normal};
            index++;
        }
    }

    return index;
}

int VulkanSpheres::generateIndices(int sectors, int stacks)
{
    int index = 0;

    for (int i = 0; i < stacks; ++i)
    {
        int stackStart = i * (sectors + 1);
        int nextStackStart = (i + 1) * (sectors + 1);

        for (int j = 0; j < sectors; ++j)
        {
            uint32_t topLeft = stackStart + j;
            uint32_t bottomLeft = nextStackStart + j;
            uint32_t topRight = stackStart + j + 1;
            uint32_t bottomRight = nextStackStart + j + 1;

            // Dreieck 1
            mIndices[index] = topLeft;
            index++;
            mIndices[index] = bottomLeft;
            index++;
            mIndices[index] = topRight;
            index++;

            // Dreieck 2
            mIndices[index] = topRight;
            index++;
            mIndices[index] = bottomLeft;
            index++;
            mIndices[index] = bottomRight;
            index++;
        }
    }

    return index;
}


void VulkanSpheres::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVerticesSize;

    createBuffer(mPhysicalDevice, mLogicalDevice, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 mVertexBuffer, mVertexBufferMemory);

    void *data;
    vkMapMemory(mLogicalDevice, mVertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mVertices, static_cast<size_t>(bufferSize));
    vkUnmapMemory(mLogicalDevice, mVertexBufferMemory);
}

void VulkanSpheres::createIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndicesSize;

    createBuffer(mPhysicalDevice, mLogicalDevice, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 mIndexBuffer, mIndexBufferMemory);

    void *data;
    vkMapMemory(mLogicalDevice, mIndexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mIndices, static_cast<size_t>(bufferSize));
    vkUnmapMemory(mLogicalDevice, mIndexBufferMemory);
}

void VulkanSpheres::createPipeline(VkRenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout, int fbWidth, int fbHeight)
{
    ShaderData vertShaderData = getShaderVertData();
    ShaderData fragShaderData = getShaderFragData();

    VkShaderModule vertModule =
        createShaderModule(mLogicalDevice, reinterpret_cast<const char*>(vertShaderData.data), vertShaderData.size);
    VkShaderModule fragModule =
        createShaderModule(mLogicalDevice, reinterpret_cast<const char*>(fragShaderData.data), fragShaderData.size);

    VkPipelineShaderStageCreateInfo vertStageInfo{};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertModule;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo{};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = fragModule;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo,
                                                      fragStageInfo};

    // configure push constants
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Shader-Stufe
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4) * 3; // model + view + proj

    // Erzeuge Vertexdaten
    auto bindingDescription = Vertex::getBindingDescription();
    VkVertexInputAttributeDescription attributeDescriptions[ATTRIBUTES_DESCRIPTION_SIZE];
    Vertex::getAttributeDescriptions(attributeDescriptions);
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &bindingDescription;
    vertexInput.vertexAttributeDescriptionCount = ATTRIBUTES_DESCRIPTION_SIZE;
    vertexInput.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(fbWidth);
    viewport.height = static_cast<float>(fbHeight);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight)};

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Pipeline-Layout (leer, keine Uniforms)
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(mLogicalDevice, &pipelineLayoutInfo, nullptr,
                               &mPipelineLayout) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create pipeline layout!");
        exit(EXIT_FAILURE);
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(mLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo,
                                  nullptr, &mPipeline) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create graphics pipeline!");
        exit(EXIT_FAILURE);
    }

    vkDestroyShaderModule(mLogicalDevice, vertModule, nullptr);
    vkDestroyShaderModule(mLogicalDevice, fragModule, nullptr);
}

//---------------------------------------------------
void VulkanSpheres::draw(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const WorldSphere* spheres, unsigned int spheresSize)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    VkBuffer vertexBuffers[] = {mVertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

    for (unsigned int j = 0; j < spheresSize; j++)
    {
        WorldSphere sphere = spheres[j];
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(sphere.getPos().x, sphere.getPos().y, sphere.getPos().z)) *
                          glm::scale(glm::mat4(1.0f), glm::vec3(sphere.getDiameter()));

        SpheresPushConstants pc{model, viewMatrix, projMatrix};

        vkCmdPushConstants(commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(SpheresPushConstants), &pc);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(SPHERE_INDICES), 1, 0, 0, 0);
    }
}
