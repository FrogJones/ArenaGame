#include "config.h"

/**
 * @brief Defines the world-space positions for all point lights in the scene.
 *
 * This array provides a centralized configuration for lighting, making it easy
 * to adjust the scene's illumination without modifying shader or engine code.
 * The order and number of lights should correspond to the `NUM_POINT_LIGHTS`
 * constant defined in `config.h`.
 */
const glm::vec3 POINT_LIGHT_POSITIONS[NUM_POINT_LIGHTS] = {
    glm::vec3(-2.65f, 1.25f, -2.85f),  // Light in the back-left corner of the room
    glm::vec3( 2.65f, 1.25f, -2.85f),  // Light in the back-right corner of the room
    glm::vec3( 2.65f, 1.25f,  2.85f),  // Light in the front-right corner of the room
    glm::vec3(-2.65f, 1.25f,  2.85f),  // Light in the front-left corner of the room
    glm::vec3( 0.0f, 1.25f, -2.85f),  // Light on the middle of the back wall
    glm::vec3( 2.85f, 1.25f,  0.0f),  // Light on the middle of the right wall
    glm::vec3( 0.0f, 1.25f,  2.85f),  // Light on the middle of the front wall
    glm::vec3(-2.9f, 1.25f,  0.0f),  // Light on the middle of the left wall
    glm::vec3( 0.0f, 1.25f,  0.0f)   // Light above the bonfire in the center
};