#include "config.h"

// Define the point light positions array
const glm::vec3 POINT_LIGHT_POSITIONS[NUM_POINT_LIGHTS] = {
    glm::vec3(-2.7f, 1.2f, -2.9f),  // Back-left corner
    glm::vec3( 2.7f, 1.2f, -2.9f),  // Back-right corner
    glm::vec3( 2.7f, 1.2f,  2.9f),  // Front-right corner
    glm::vec3(-2.7f, 1.2f,  2.9f),  // Front-left corner
    glm::vec3( 0.0f, 1.2f, -2.9f),  // Mid-back
    glm::vec3( 2.9f, 1.2f,  0.0f),  // Mid-right
    glm::vec3( 0.0f, 1.2f,  2.9f),  // Mid-front
    glm::vec3(-2.9f, 1.2f,  0.0f)   // Mid-left
};