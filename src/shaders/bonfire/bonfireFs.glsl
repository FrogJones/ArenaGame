#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    float shininess;
    float emissiveStrength;
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
uniform float time;

uniform float fogNear;
uniform float fogFar;
uniform vec3 fogColor;

const float COLOR_LEVELS = 32.0;
const float LIGHTING_LEVELS = 8.0;
const float BRIGHTNESS_THRESHOLD = 0.7;
const float EMISSIVE_FOG_FACTOR = 0.5;
const float DITHER_AMOUNT = 0.01;
const float DITHER_PATTERN = 2.0;

vec3 quantizeColor(vec3 color, float levels) {
    return floor(color * levels) / levels;
}

vec3 calculateTorchLighting(vec3 normal, vec3 texColor) {
    vec3 lightDir = normalize(-dirLight.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    diff = floor(diff * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
    vec3 ambient = dirLight.ambient * texColor * 0.3;
    vec3 diffuse = dirLight.diffuse * diff * texColor * 0.5;
    
    return ambient + diffuse;
}

float calculateFlicker(float time) {
    float flicker = sin(time * 8.0) * 0.1 +
                   sin(time * 12.0) * 0.05 +
                   sin(time * 16.0) * 0.03;
    return clamp(0.8 + flicker, 0.5, 1.0);
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 texColor = texture(material.diffuse, TexCoords).rgb;
    
    vec3 result = calculateTorchLighting(norm, texColor);
    
    float flicker = calculateFlicker(time);
    vec3 emissive = texColor * material.emissiveStrength * flicker;
    
    float brightness = dot(texColor, vec3(0.299, 0.587, 0.114));
    if (brightness > BRIGHTNESS_THRESHOLD) {
        emissive += vec3(1.0, 0.6, 0.1) * material.emissiveStrength * flicker * 0.8;
    }
    
    result += emissive;
    
    result = quantizeColor(result, COLOR_LEVELS);
    
    float distance = length(viewPos - FragPos);
    float fogFactor = clamp((fogFar - distance) / (fogFar - fogNear), 0.0, 1.0);
    
    fogFactor = floor(fogFactor * LIGHTING_LEVELS) / LIGHTING_LEVELS;
    
    float emissiveFactor = material.emissiveStrength * EMISSIVE_FOG_FACTOR;
    fogFactor = clamp(fogFactor + emissiveFactor, 0.0, 1.0);
    
    result = mix(fogColor, result, fogFactor);
    
    vec2 screenPos = gl_FragCoord.xy;
    float dither = mod(screenPos.x + screenPos.y, DITHER_PATTERN) * DITHER_AMOUNT;
    result += vec3(dither);
    
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}