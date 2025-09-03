#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

constexpr uint32_t LIGHTS_AMOUNT = 3;

struct Light
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
};

class VulkanCamera
{
public:
    VulkanCamera();
    
    void updateCamera(float time, int fbWidth, int fbHeight);
    
    // Getters for matrices
    const glm::mat4& getViewMatrix() const { return view; }
    const glm::mat4& getProjectionMatrix() const { return proj; }
    
    // Getters for lights
    Light* getLights() { return defaultLights; }
    int getLightsSize() const { return LIGHTS_AMOUNT; }
    
    // Getters for animation state
    float getLastSwitchTime() const { return lastSwitchTime; }
    void setLastSwitchTime(float time) { lastSwitchTime = time; }

private:
    // camera matrix
    glm::mat4 view;
    glm::mat4 proj;
    
    // lights for the scenery
    Light defaultLights[LIGHTS_AMOUNT];

    // camera orbit
    float orbitRadius = 3.0f;
    float orbitHeight = 2.0f;
    float orbitSpeed = 0.05f;
    glm::vec3 orbitAxis = glm::normalize(glm::vec3(0.1, 1.0f, 0.1));

    // animation
    float lastSwitchTime = 0;
    uint32_t rng_state = 0xDEADBEEF;
    
    void setView(glm::vec3 eye, int fbWidth, int fbHeight);
    
    uint32_t rand_u32();
    float rand_float_range(float min, float max);
    int rand_int_range(int min, int max);
};