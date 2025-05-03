#version 450

// Eingänge vom Vertex Shader
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 fragNormal;

// Ausgabe an den Framebuffer
layout(location = 0) out vec4 outColor;

// Uniform Buffer für Lichtdaten (std140 Layout!)
struct Light {
    vec4 position;
    vec4 color;
};

layout(binding = 0, std140) uniform Lights {
    Light lights[3];
};

void main() {
    vec3 normal = normalize(fragNormal);  // Normale interpoliert von Vertices
    vec3 result = vec3(0.0);              // Startfarbe schwarz

    // 🔆 Ambient Light: 20 % der Materialfarbe immer aktiv
    vec3 ambient = 0.2 * fragColor;
    result += ambient;

    for (int i = 0; i < 3; i++) {
        // Richtung von Fragment zur Lichtquelle
        vec3 lightDir = normalize(lights[i].position.xyz - fragPos);
        vec3 lightColor = lights[i].color.xyz;

        // Lambert-Beleuchtung: wie "schräg" trifft das Licht?
        float diff = max(dot(normal, lightDir), 0.0);

        // Farbe = Materialfarbe * Lichtfarbe * Intensität
        result += fragColor * lightColor * diff;
    }

    outColor = vec4(result, 1.0); // finaler RGBA-Ausgabewert
}
