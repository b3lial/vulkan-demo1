#include "ShaderData.hpp"
#include "Logger.hpp"

//---------------------------------------------------
VkShaderModule createShaderModule(VkDevice &device, const char *code, size_t size)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
        VK_SUCCESS)
    {
        LOG_DEBUG("Failed to create shader module!");
        exit(EXIT_FAILURE);
    }
    return shaderModule;
}

//---------------------------------------------------
char *readFile(const char *filename, size_t *size)
{
    LOG_DEBUG(filename);

    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        LOG_DEBUG("failed to open file!");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file); // oder: fseek(file, 0, SEEK_SET);

    if (fileSize < 0)
    {
        LOG_DEBUG("ftell failed!");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    *size = static_cast<size_t>(fileSize);
    char *buffer = new char[*size];

    size_t bytesRead = fread(buffer, 1, *size, file);
    fclose(file);

    if (bytesRead != *size)
    {
        LOG_DEBUG("fread failed!");
        delete[] buffer;
        exit(EXIT_FAILURE);
    }

    return buffer;
}
