#include "VulkanCube.hpp"
#include "Logger.hpp"
#include "EmbeddedShaders.hpp"

#include <memory.h>

VulkanCube::VulkanCube() : mVerticesSize(0), mIndicesSize(0)
{
    // Initialize the static indices for cube faces
    // Each face uses 2 triangles (6 indices)
    for (int face = 0; face < 6; ++face) {
        int baseVertex = face * 4;
        int baseIndex = face * 6;
        
        // First triangle (0, 1, 2)
        mIndices[baseIndex + 0] = baseVertex + 0;
        mIndices[baseIndex + 1] = baseVertex + 1;
        mIndices[baseIndex + 2] = baseVertex + 2;
        
        // Second triangle (0, 2, 3)
        mIndices[baseIndex + 3] = baseVertex + 0;
        mIndices[baseIndex + 4] = baseVertex + 2;
        mIndices[baseIndex + 5] = baseVertex + 3;
    }
    mIndicesSize = 36; // 6 faces * 6 indices per face
}

void VulkanCube::generateVerticesFromSides(const WorldCube::Side* sides, double edgeLength)
{
    mVerticesSize = 0;
    
    for (int i = 0; i < 6; ++i) {
        glm::dvec3 corners[4];
        sides[i].getCorners(corners, edgeLength);
        
        // Convert corners to vertices and add them
        for (int j = 0; j < 4; ++j) {
            mVertices[mVerticesSize].pos = glm::vec3(corners[j]);
            mVertices[mVerticesSize].color = glm::vec3(0.5f, 0.5f, 1.0f); // Light blue for transparency effect
            mVertices[mVerticesSize].normal = glm::vec3(sides[i].normal());
            mVerticesSize++;
        }
    }
}

void VulkanCube::createVertexBuffer()
{

}

void VulkanCube::createIndexBuffer()
{

}

/**
 * - Uses the same shaders as VulkanSpheres (shader_vert_spv and shader_frag_spv)
 * - Disabled face culling (VK_CULL_MODE_NONE) so both front and back faces are rendered for proper transparency
 * - Enabled alpha blending with standard transparency blend factors:
 *   - srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA
 *   - dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
 * - Uses the same push constants structure as VulkanSpheres for model/view/projection matrices
 * - Same vertex input layout using the Vertex struct from ShaderData.hpp
 */
void VulkanCube::createPipeline(VkRenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout, int fbWidth, int fbHeight)
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

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo, fragStageInfo};

    // configure push constants
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4) * 3; // model + view + proj

    // Vertex input description
    auto bindingDescription = Vertex::getBindingDescription();
    VkVertexInputAttributeDescription attributeDescriptions[ATTRIBUTES_DESCRIPTION_SIZE];
    Vertex::getAttributeDescriptions(attributeDescriptions);
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 1;
    vertexInput.pVertexBindingDescriptions = &bindingDescription;
    vertexInput.vertexAttributeDescriptionCount = ATTRIBUTES_DESCRIPTION_SIZE;
    vertexInput.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
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
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE; // Disable culling for transparent cube
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Enable alpha blending for transparency
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(mLogicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create cube pipeline layout!");
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

    if (vkCreateGraphicsPipelines(mLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create cube graphics pipeline!");
        exit(EXIT_FAILURE);
    }

    vkDestroyShaderModule(mLogicalDevice, vertModule, nullptr);
    vkDestroyShaderModule(mLogicalDevice, fragModule, nullptr);
}

void VulkanCube::draw(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const WorldCube::Side* sides, double edgeLength)
{

}
