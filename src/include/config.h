#ifndef CONFIG_H
#define CONFIG_H

#include <glm/glm.hpp>

// Screen constants
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// Movement constants
const float STEP_COOLDOWN = 0.6f;
const float BOB_AMOUNT = 0.05f;
const float BOB_SPEED = 10.0f;
const float MOVEMENT_THRESHOLD = 0.01f;

// Lighting constants
const int NUM_POINT_LIGHTS = 9;
const int BONFIRE_LIGHT_INDEX = 8;
extern const glm::vec3 POINT_LIGHT_POSITIONS[NUM_POINT_LIGHTS];

// Directional light values
const glm::vec3 DIR_LIGHT_DIRECTION = glm::vec3(0.0f, -1.0f, 0.0f);
const glm::vec3 DIR_LIGHT_AMBIENT = glm::vec3(0.7f, 0.65f, 0.55f);
const glm::vec3 DIR_LIGHT_DIFFUSE = glm::vec3(0.9f, 0.85f, 0.75f);
const glm::vec3 DIR_LIGHT_SPECULAR = glm::vec3(0.0f, 0.0f, 0.0f);

// Bonfire light values
const glm::vec3 BONFIRE_AMBIENT_BASE = glm::vec3(0.15f, 0.08f, 0.03f);
const glm::vec3 BONFIRE_DIFFUSE_BASE = glm::vec3(2.5f, 1.3f, 0.5f);
const glm::vec3 BONFIRE_SPECULAR = glm::vec3(0.0f, 0.0f, 0.0f);

// Regular light values
const glm::vec3 REGULAR_LIGHT_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

// Light attenuation
const float LIGHT_CONSTANT = 1.0f;
const float BONFIRE_LINEAR = 0.35f;
const float BONFIRE_QUADRATIC = 1.2f;
const float REGULAR_LINEAR = 0.0f;
const float REGULAR_QUADRATIC = 0.0f;

// Flicker parameters
const float FLICKER_BASE = 0.9f;
const float FLICKER_AMPLITUDE = 0.1f;
const float FLICKER_FREQ1 = 8.0f;
const float FLICKER_FREQ2 = 15.0f;
const float FLICKER_PHASE1 = 1.5f;
const float FLICKER_PHASE2 = 0.8f;

// Material properties
const float MATERIAL_SHININESS = 4.0f;
const float MATERIAL_ALPHA = 1.0f;

// Fog parameters
const float FOG_NEAR = 4.0f;
const float FOG_FAR = 7.0f;
const glm::vec3 FOG_COLOR = glm::vec3(0.02f, 0.02f, 0.04f);

// Torch/Emissive lighting
const glm::vec3 TORCH_DIR_AMBIENT = glm::vec3(0.01f, 0.005f, 0.002f);
const glm::vec3 TORCH_DIR_DIFFUSE = glm::vec3(0.1f, 0.08f, 0.05f);
const float TORCH_SHININESS = 2.0f;
const float TORCH_EMISSIVE_STRENGTH = 1.5f;

// Camera constants
const float CAMERA_HEIGHT = 1.0f;
const float BOUNDARY_LIMIT = 2.8f;
const float PITCH_CONSTRAINT_MAX = 89.0f;
const float PITCH_CONSTRAINT_MIN = -89.0f;
const float ZOOM_MIN = 1.0f;
const float ZOOM_MAX = 45.0f;

#endif