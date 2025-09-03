#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    float shininess;
    float emissiveStrength;  // How much the torch glows
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;
uniform float time; // For flickering effect

// PS1-style fog uniforms
uniform float fogNear;
uniform float fogFar;
uniform vec3 fogColor;

// PS1-style quantization levels
const float COLOR_LEVELS = 32.0;
const float LIGHTING_LEVELS = 8.0;

// Function to quantize colors to PS1-style color depth
vec3 quantizeColor(vec3 color, float levels) {
    return floor(color * levels) / levels;
}

// Simple torch lighting calculation
vec3 calculateTorchLighting(vec3 normal, vec3 texColor) {
    vec3 lightDir = normalize(-dirLight.direction);
    
    // Simple diffuse lighting, quantized
    float diff = max(dot(normal, lightDir), 0.0);
    diff = floor(diff * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
    // Basic ambient and diffuse
    vec3 ambient = dirLight.ambient * texColor * 0.3;
    vec3 diffuse = dirLight.diffuse * diff * texColor * 0.5; // Reduced since torch is self-lit
    
    return ambient + diffuse;
}

// Flickering effect for torch flame/emissive parts
float calculateFlicker(float time) {
    // Combine multiple sine waves for natural-looking flicker
    float flicker = sin(time * 8.0) * 0.1 +
                   sin(time * 12.0) * 0.05 +
                   sin(time * 16.0) * 0.03;
    return clamp(0.8 + flicker, 0.5, 1.0);
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 texColor = texture(material.diffuse, TexCoords).rgb;
    
    // Calculate basic lighting
    vec3 result = calculateTorchLighting(norm, texColor);
    
    // Add emissive glow (the torch's own light)
    float flicker = calculateFlicker(time);
    vec3 emissive = texColor * material.emissiveStrength * flicker;
    
    // For torch flame areas (assuming they're brighter in the texture), increase emissive
    float brightness = dot(texColor, vec3(0.299, 0.587, 0.114)); // Luminance
    if (brightness > 0.7) {
        // Bright areas (flame) glow more and flicker more dramatically
        emissive += vec3(1.0, 0.6, 0.1) * material.emissiveStrength * flicker * 0.8;
    }
    
    result += emissive;
    
    // Quantize the final color to simulate PS1's limited color depth
    result = quantizeColor(result, COLOR_LEVELS);
    
    // Calculate PS1-style fog
    float distance = length(viewPos - FragPos);
    float fogFactor = clamp((fogFar - distance) / (fogFar - fogNear), 0.0, 1.0);
    
    // Quantize fog factor for PS1-style stepped fog
    fogFactor = floor(fogFactor * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
    // Emissive objects should be less affected by fog (they glow through it)
    float emissiveFactor = material.emissiveStrength * 0.5;
    fogFactor = clamp(fogFactor + emissiveFactor, 0.0, 1.0);
    
    // Mix lit color with fog color
    result = mix(fogColor, result, fogFactor);
    
    // Optional: Add slight dithering pattern for more authentic PS1 look
    vec2 screenPos = gl_FragCoord.xy;
    float dither = mod(screenPos.x + screenPos.y, 2.0) * 0.01;
    result += vec3(dither);
    
    // Clamp to prevent over-bright colors
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}