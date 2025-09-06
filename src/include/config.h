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
extern const glm::vec3 POINT_LIGHT_POSITIONS[NUM_POINT_LIGHTS];

// Camera constants
const float CAMERA_HEIGHT = 1.0f;
const float BOUNDARY_LIMIT = 2.8f;

#endif