#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "gameState.h"
#include "config.h"

GameState::GameState() 
    : camera(glm::vec3(0.0f, 0.0f, 3.0f)),
      deltaTime(0.0f),
      lastFrame(0.0f),
      lastX(SCR_WIDTH / 2.0f),
      lastY(SCR_HEIGHT / 2.0f),
      firstMouse(true),
      cursorLocked(true),
      awaitingRelock(false),
      lastCameraPos(camera.Position),
      stepCooldown(0.0f),
      bobTimer(0.0f),
      projection(glm::mat4(1.0f))
{
    // Initialize projection matrix
    float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.01f, 20.0f);

    interactionSystem.initialize();
}

void GameState::updateTiming() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void GameState::updateMovement() {
    // Store the base camera position before applying bob
    glm::vec3 baseCameraPos = camera.Position;
    baseCameraPos.y = CAMERA_HEIGHT; // Fixed height
    
    // Apply movement boundaries BEFORE bobbing
    if (baseCameraPos.x > BOUNDARY_LIMIT) baseCameraPos.x = BOUNDARY_LIMIT;
    if (baseCameraPos.x < -BOUNDARY_LIMIT) baseCameraPos.x = -BOUNDARY_LIMIT;
    if (baseCameraPos.z > BOUNDARY_LIMIT) baseCameraPos.z = BOUNDARY_LIMIT;
    if (baseCameraPos.z < -BOUNDARY_LIMIT) baseCameraPos.z = -BOUNDARY_LIMIT;
    
    // Update camera position with boundaries applied
    camera.Position = baseCameraPos;
    
    // Calculate movement distance for bobbing and audio
    float moveDistance = glm::length(camera.Position - lastCameraPos);
    
    // Calculate bob only if moving
    float bobOffset = 0.0f;
    if (moveDistance > MOVEMENT_THRESHOLD) {
        bobTimer += deltaTime * BOB_SPEED;
        bobOffset = sin(bobTimer) * BOB_AMOUNT;
    } else {
        // Gradually reset bob timer when standing still
        bobTimer = 0.0f;
    }
    
    // Apply bob to camera position
    camera.Position.y = CAMERA_HEIGHT + bobOffset;
    
    // Update last position for next frame (use the base position without bob)
    lastCameraPos = baseCameraPos;
    
    // Update step cooldown
    if (stepCooldown > 0.0f) {
        stepCooldown -= deltaTime;
    }
}

void GameState::updateInteraction(GLFWwindow* window) {
    // Reset prompt state
    showInteractionPrompt = false;
    interactionText.clear();

    // Check all interactables and set prompt text if close enough
    bool near = interactionSystem.CheckInteractions(camera.Position, interactionText);
    showInteractionPrompt = near;

    // If near and E pressed, trigger interaction (debounced)
    if (near) {
        int state = glfwGetKey(window, GLFW_KEY_E);
        if (state == GLFW_PRESS && !awaitingRelock) {
            bool interacted = interactionSystem.HandleInteraction(camera.Position);
            awaitingRelock = true;

            if (interacted) {
                // consume the prompt immediately and clear text so it won't reappear
                showInteractionPrompt = false;
                interactionText.clear();
            }
        } else if (state == GLFW_RELEASE) {
            // allow next press
            awaitingRelock = false;
        }
    } else {
        // ensure button state resets when away
        awaitingRelock = false;
    }
}