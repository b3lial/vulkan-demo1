#include "VulkanCamera.hpp"

VulkanCamera::VulkanCamera()
    : lights(nullptr), lightsSize(0)
{
}

void VulkanCamera::setLights(Light l[], int size)
{
    lights = l;
    lightsSize = size;
}

void VulkanCamera::setView(glm::vec3 eye, int fbWidth, int fbHeight)
{
    view = glm::lookAt(eye,                        // Eye
                       glm::vec3(0.0f),            // Center
                       glm::vec3(0.0f, 1.0f, 0.0f) // Up
    );

    proj = glm::perspective(glm::radians(45.0f), fbWidth / (float)fbHeight, 0.1f,
                            3000.0f);
    proj[1][1] *= -1; // Vulkan Y-Korrektur
}

void VulkanCamera::updateCamera(float time, int fbWidth, int fbHeight)
{
    // change view every 8 seconds for more dynamic
    if (lastSwitchTime == 0)
    {
        lastSwitchTime = time;
    }
    else if (time - lastSwitchTime > 8)
    {
        lastSwitchTime = time;

        // change view
        orbitRadius = rand_float_range(2.5f, 6.0f);
        orbitHeight = rand_float_range(1.0f, 3.0f);
        orbitSpeed = rand_float_range(0.02f, 0.05f);

        float axisX = rand_float_range(-0.3f, 0.3f);
        float axisZ = rand_float_range(-0.3f, 0.3f);
        orbitAxis = glm::normalize(glm::vec3(axisX, 1.0f, axisZ));

        if (rand_int_range(0, 1) == 1)
        {
            orbitSpeed *= -1.0f;
        }
    }

    // Calculate camera position
    float angle = glm::two_pi<float>() * orbitSpeed * time;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, orbitAxis);
    glm::vec3 eye = glm::vec3(
        rotation * glm::vec4(orbitRadius, orbitHeight, 0.0f, 1.0f));
    
    // Update view and projection matrices
    setView(eye, fbWidth, fbHeight);
}

uint32_t VulkanCamera::rand_u32()
{
    rng_state = rng_state * 1664525 + 1013904223;
    return rng_state;
}

float VulkanCamera::rand_float_range(float min, float max)
{
    return min + (rand_u32() / (float)UINT32_MAX) * (max - min);
}

int VulkanCamera::rand_int_range(int min, int max)
{
    return min + (rand_u32() % (max - min + 1));
}