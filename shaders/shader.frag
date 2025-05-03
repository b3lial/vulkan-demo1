#version 450

// Eingänge vom Vertex Shader
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 fragNormal;

// Ausgabe an den Framebuffer
layout(location = 0) out vec4 outColor;

// Uniform Buffer für Lichtdaten (std140 Layout!)
layout(binding = 0, std140) uniform Lights {
    vec4 lightPos[2];    // .xyz = Position, .w ungenutzt
    vec4 lightColor[2];  // .xyz = Farbe, .w ungenutzt
};

void main() {
    vec3 normal = normalize(fragNormal);  // Normale interpoliert von Vertices
    vec3 result = vec3(0.0);              // Startfarbe schwarz

    for (int i = 0; i < 2; i++) {
        // Richtung von Fragment zur Lichtquelle
        vec3 lightDir = normalize(lightPos[i].xyz - fragPos);

        // Lambert-Beleuchtung: wie "schräg" trifft das Licht?
        float diff = max(dot(normal, lightDir), 0.0);

        // Farbe = Materialfarbe * Lichtfarbe * Intensität
        result += fragColor * lightColor[i].xyz * diff;
    }

    outColor = vec4(result, 1.0); // finaler RGBA-Ausgabewert
}
