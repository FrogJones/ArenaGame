#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    float shininess;
    // Removed specular sampler2D since PS1 rarely used specular maps
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 8

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

// PS1-style fog uniforms
uniform float fogNear;
uniform float fogFar;
uniform vec3 fogColor;

// PS1-style quantization levels
const float COLOR_LEVELS = 32.0;  // Reduce this for more banding (try 16.0 or 8.0)
const float LIGHTING_LEVELS = 8.0; // Quantize lighting calculations

// Function to quantize colors to PS1-style color depth
vec3 quantizeColor(vec3 color, float levels) {
    return floor(color * levels) / levels;
}

// Simple PS1-style directional light (no fancy Phong)
vec3 CalcPS1DirLight(DirLight light, vec3 normal) {
    vec3 lightDir = normalize(-light.direction);
    
    // Simple dot product for lighting, quantized
    float diff = max(dot(normal, lightDir), 0.0);
    diff = floor(diff * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
    // Sample texture
    vec3 texColor = texture(material.diffuse, TexCoords).rgb;
    
    // PS1 didn't have sophisticated ambient/diffuse separation
    // Just blend between dark and lit based on the quantized lighting
    vec3 ambient = light.ambient * texColor * 0.3; // Darker ambient
    vec3 diffuse = light.diffuse * diff * texColor;
    
    return ambient + diffuse;
}

// PS1-style point light with harsh falloff
vec3 CalcPS1PointLight(PointLight light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Quantized diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    diff = floor(diff * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
    // Harsh distance attenuation (PS1 had very simple distance calculations)
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Quantize the attenuation for that stepped falloff look
    attenuation = floor(attenuation * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
    vec3 texColor = texture(material.diffuse, TexCoords).rgb;
    
    vec3 ambient = light.ambient * texColor * 0.2;
    vec3 diffuse = light.diffuse * diff * texColor;
    
    return (ambient + diffuse) * attenuation;
}

void main() {
    vec3 norm = normalize(Normal);
    
    // Start with directional lighting
    vec3 result = CalcPS1DirLight(dirLight, norm);
    
    // Add point lights (now all 8 torches)
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPS1PointLight(pointLights[i], norm, FragPos);
    }
    
    // Quantize the final color to simulate PS1's limited color depth
    result = quantizeColor(result, COLOR_LEVELS);
    
    // Calculate PS1-style fog
    float distance = length(viewPos - FragPos);
    float fogFactor = clamp((fogFar - distance) / (fogFar - fogNear), 0.0, 1.0);
    
    // Quantize fog factor for PS1-style stepped fog
    fogFactor = floor(fogFactor * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
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