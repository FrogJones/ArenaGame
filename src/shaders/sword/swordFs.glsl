#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    float shininess;
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

#define NR_POINT_LIGHTS 9

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

// PS1-style quantization levels
const float COLOR_LEVELS = 32.0;
const float LIGHTING_LEVELS = 8.0;

vec3 quantizeColor(vec3 color, float levels) {
    return floor(color * levels) / levels;
}

vec3 CalcPS1DirLight(DirLight light, vec3 normal) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    diff = floor(diff * LIGHTING_LEVELS) / LIGHTING_LEVELS;

    vec3 texColor = texture(material.diffuse, TexCoords).rgb;
    vec3 ambient = light.ambient * texColor * 0.3;
    vec3 diffuse = light.diffuse * diff * texColor;

    return ambient + diffuse;
}

vec3 CalcPS1PointLight(PointLight light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    diff = floor(diff * LIGHTING_LEVELS) / LIGHTING_LEVELS;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    attenuation = floor(attenuation * LIGHTING_LEVELS) / LIGHTING_LEVELS;

    vec3 texColor = texture(material.diffuse, TexCoords).rgb;
    vec3 ambient = light.ambient * texColor * 0.2;
    vec3 diffuse = light.diffuse * diff * texColor;

    return (ambient + diffuse) * attenuation;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Directional light
    vec3 dirColor = CalcPS1DirLight(dirLight, norm);
    vec3 dirLightDir = normalize(-dirLight.direction);
    float dirSpec = pow(max(dot(viewDir, reflect(-dirLightDir, norm)), 0.0), material.shininess);
    vec3 dirSpecular = dirLight.specular * dirSpec;
    result += dirColor + dirSpecular;

    // Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        vec3 pointColor = CalcPS1PointLight(pointLights[i], norm, FragPos);
        vec3 pointDir = normalize(pointLights[i].position - FragPos);
        float pointSpec = pow(max(dot(viewDir, reflect(-pointDir, norm)), 0.0), material.shininess);
        vec3 pointSpecular = pointLights[i].specular * pointSpec;
        result += pointColor + pointSpecular;
    }

    // Quantize final color
    result = quantizeColor(result, COLOR_LEVELS);

    // Optional dithering
    vec2 screenPos = gl_FragCoord.xy;
    float dither = mod(screenPos.x + screenPos.y, 2.0) * 0.01;
    result += vec3(dither);

    result = clamp(result, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
}
