#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <glm/glm.hpp>
#include <camera.h>
#include <string>
#include <GLFW/glfw3.h>
#include "interactionSystem.h"
#include "inventory.h"

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
    bool eKeyPressed;
    bool tabKeyPressed;
    
    // Movement and effects
    glm::vec3 lastCameraPos;
    float stepCooldown;
    float bobTimer;
    
    // Projection matrix
    glm::mat4 projection;

    // Interaction
    bool showInteractionPrompt = false;
    std::string interactionText = "";
    std::string interactionPopup = "";
    float popupTimer = 0.0f;

    // UI State
    bool showMenu = false;
    bool showInventory = false;
    bool showCrosshair = true;
    bool showItemDescription = false;
    std::string selectedItemDescription = "";

    // New: sword/bonfire state exposed to other systems
    bool hasBrokenSword = false;
    std::string swordType;

    InteractionSystem interactionSystem;

    Inventory inventory;
    
    GameState();
    void updateTiming();
    void updateMovement();

    void updateInteraction(GLFWwindow* window);
    void updatePopup();
};

#endif