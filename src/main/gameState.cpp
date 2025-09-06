/**
 * @file gameState.cpp
 * @brief Manages the dynamic state of the game world, including camera, timing, and player interactions.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "gameState.h"
#include "config.h"

/**
 * @brief Constructs the GameState, setting initial values for all state variables.
 */
GameState::GameState() 
    : camera(glm::vec3(0.0f, 0.0f, 3.0f)),
      deltaTime(0.0f),
      lastFrame(0.0f),
      lastX(SCR_WIDTH / 2.0f),
      lastY(SCR_HEIGHT / 2.0f),
      firstMouse(true),
      cursorLocked(true),
      awaitingRelock(false),
      eKeyPressed(false),
      tabKeyPressed(false),
      lastCameraPos(camera.Position),
      stepCooldown(0.0f),
      bobTimer(0.0f),
      projection(glm::mat4(1.0f))
{
    // Initialize the projection matrix with the screen dimensions and camera properties.
    float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.01f, 20.0f);

    // Initialize the interaction system.
    interactionSystem.initialize();
}

/**
 * @brief Updates frame timing variables (deltaTime, lastFrame).
 *
 * Should be called once per frame to ensure consistent physics and movement speed.
 */
void GameState::updateTiming() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

/**
 * @brief Updates player movement, including boundary checks and head bob effect.
 */
void GameState::updateMovement() {
    // Store the camera's position before applying any view-bobbing effects.
    glm::vec3 baseCameraPos = camera.Position;
    baseCameraPos.y = CAMERA_HEIGHT; // Enforce a fixed height from the ground.
    
    // Clamp the player's position to the defined world boundaries.
    if (baseCameraPos.x > BOUNDARY_LIMIT) baseCameraPos.x = BOUNDARY_LIMIT;
    if (baseCameraPos.x < -BOUNDARY_LIMIT) baseCameraPos.x = -BOUNDARY_LIMIT;
    if (baseCameraPos.z > BOUNDARY_LIMIT) baseCameraPos.z = BOUNDARY_LIMIT;
    if (baseCameraPos.z < -BOUNDARY_LIMIT) baseCameraPos.z = -BOUNDARY_LIMIT;
    
    // Apply the bounded position back to the camera.
    camera.Position = baseCameraPos;
    
    // Calculate how far the player has moved since the last frame.
    float moveDistance = glm::length(camera.Position - lastCameraPos);
    
    // Apply a sinusoidal head bob effect if the player is moving.
    float bobOffset = 0.0f;
    if (moveDistance > MOVEMENT_THRESHOLD) {
        bobTimer += deltaTime * BOB_SPEED;
        bobOffset = sin(bobTimer) * BOB_AMOUNT;
    } else {
        // Reset the bob timer when stationary to prevent a sudden jump on the next movement.
        bobTimer = 0.0f;
    }
    
    // Apply the calculated bob offset to the camera's final Y position.
    camera.Position.y = CAMERA_HEIGHT + bobOffset;
    
    // Store this frame's position to calculate movement on the next frame.
    lastCameraPos = baseCameraPos;
    
    // Decrement the step sound cooldown timer.
    if (stepCooldown > 0.0f) {
        stepCooldown -= deltaTime;
    }
}

/**
 * @brief Checks for and handles player interactions with objects in the world.
 * @param window The active GLFW window, used for key state queries.
 */
void GameState::updateInteraction(GLFWwindow* window) {
    // Reset interaction prompt state for the new frame.
    showInteractionPrompt = false;
    interactionText.clear();

    // Check if the player is near any interactable objects.
    bool isNearObject = interactionSystem.CheckInteractions(camera.Position, interactionText);
    showInteractionPrompt = isNearObject;

    // If near an object, check if the interaction key ('E') is pressed.
    if (isNearObject) {
        int state = glfwGetKey(window, GLFW_KEY_E);
        if (state == GLFW_PRESS && !eKeyPressed) {
            // A single-press debounce mechanism.
            eKeyPressed = true; 
            
            // Handle the interaction and get the popup message.
            bool interacted = interactionSystem.HandleInteraction(camera.Position, interactionPopup);

            if (interacted) {
                // Immediately hide the prompt to provide instant feedback.
                showInteractionPrompt = false;
                interactionText.clear();
                
                // Start the popup timer (3 seconds)
                popupTimer = 3.0f;
            }
        } else if (state == GLFW_RELEASE) {
            // Allow the key to be pressed again on the next frame.
            eKeyPressed = false;
        }
    } else {
        // Ensure the key can be pressed if the player moves away and then back.
        eKeyPressed = false;
    }
}

/**
 * @brief Updates the popup timer and clears the popup when it expires.
 */
void GameState::updatePopup() {
    if (popupTimer > 0.0f) {
        popupTimer -= deltaTime;
        if (popupTimer <= 0.0f) {
            interactionPopup.clear();
            popupTimer = 0.0f;
        }
    }
}