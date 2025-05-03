#version 450

layout(location = 0) in vec3 inPosition;
layout(push_constant) uniform PushConstants {
    mat4 view;
    mat4 proj;
} pc;

void main() {
    gl_Position = pc.proj * pc.view * vec4(inPosition, 1.0);
}