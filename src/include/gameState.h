#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <glm/glm.hpp>
#include <camera.h>
#include <string>
#include <GLFW/glfw3.h>
#include "interactionSystem.h"

class GameState {
public:
    // Camera
    Camera camera;
    
    // Timing
    float deltaTime;
    float lastFrame;
    
    // Mouse handling
    float lastX, lastY;
    bool firstMouse;
    bool cursorLocked;
    bool awaitingRelock;
    
    // Movement and effects
    glm::vec3 lastCameraPos;
    float stepCooldown;
    float bobTimer;
    
    // Projection matrix
    glm::mat4 projection;

    // Interaction
    bool showInteractionPrompt = false;
    std::string interactionText = "";

    InteractionSystem interactionSystem;
    
    GameState();
    void updateTiming();
    void updateMovement();

    void updateInteraction(GLFWwindow* window);
};

#endif