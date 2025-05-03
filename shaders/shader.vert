#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec3 fragNormal;

// Push Constant für Model-Matrix
layout(push_constant) uniform PushConstants {
    mat4 model;
} pc;

void main() {
    gl_Position = pc.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragNormal = inNormal;
    fragPos = inPosition;
}
