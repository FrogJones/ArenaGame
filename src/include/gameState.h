#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <glm/glm.hpp>
#include <camera.h>

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
    
    GameState();
    void updateTiming();
    void updateMovement();
};

#endif