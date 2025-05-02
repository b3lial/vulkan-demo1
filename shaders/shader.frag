#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

// Lichtparameter (Hardcoded für den Anfang)
const vec3 lightPos = vec3(2.0, 2.0, 2.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main() {
    // Richtung von Punkt zur Lichtquelle
    vec3 lightDir = normalize(lightPos - fragPos);

    // Beleuchtung nach Lambert (diffuse)
    float diff = max(dot(normalize(fragNormal), lightDir), 0.0);
    vec3 litColor = fragColor * lightColor * diff;

    outColor = vec4(litColor, 1.0);
}
