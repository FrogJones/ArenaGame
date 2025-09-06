/**
 * @file inputHandler.cpp
 * @brief Handles all raw user input from keyboard and mouse and translates it into game actions.
 */

#include "inputHandler.h"
#include "config.h"
#include <glm/gtc/matrix_transform.hpp>

// Define projection plane constants if not defined in config.h
#ifndef PROJECTION_FAR_PLANE
#define PROJECTION_FAR_PLANE 100.0f
#endif

#ifndef PROJECTION_NEAR_PLANE
#define PROJECTION_NEAR_PLANE 0.1f
#endif

// A static instance is used to allow GLFW's C-style callbacks to access the handler's methods.
InputHandler* InputHandler::instance = nullptr;

/**
 * @brief Constructs the InputHandler, linking it to the main GameState.
 * @param state A pointer to the central game state object.
 */
InputHandler::InputHandler(GameState* state) : gameState(state) {
    instance = this; // Set the static instance for GLFW callbacks.
}

/**
 * @brief GLFW callback for window resize events.
 *
 * Updates the OpenGL viewport and the camera's projection matrix to match the new window size.
 */
void InputHandler::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    if (instance && instance->gameState) {
        float aspect = (float)width / (float)height;
        instance->gameState->projection = glm::perspective(
            glm::radians(instance->gameState->camera.Zoom), 
            aspect, 
            PROJECTION_NEAR_PLANE, 
            PROJECTION_FAR_PLANE
        );
    }
}

/**
 * @brief GLFW callback for mouse movement.
 *
 * Calculates the mouse's offset since the last frame and passes it to the camera
 * to handle rotation. Ignores input if the cursor is not locked to the window.
 */
void InputHandler::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    // Ignore mouse movement when the cursor is unlocked (e.g., in a menu).
    if (!instance || !instance->gameState || !instance->gameState->cursorLocked) {
        return;
    }
    
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // On the first mouse input, snap the 'last' position to the current one to prevent a large jump.
    if (instance->gameState->firstMouse) {
        instance->gameState->lastX = xpos;
        instance->gameState->lastY = ypos;
        instance->gameState->firstMouse = false;
    }

    // Calculate the offset from the last frame.
    float xoffset = xpos - instance->gameState->lastX;
    float yoffset = instance->gameState->lastY - ypos; // Reversed since y-coordinates go from bottom to top.
    
    // Update the last position for the next frame.
    instance->gameState->lastX = xpos;
    instance->gameState->lastY = ypos;

    // Process the movement.
    instance->gameState->camera.ProcessMouseMovement(xoffset, yoffset);
}

/**
 * @brief GLFW callback for mouse button presses.
 *
 * Handles re-locking the cursor to the window after it has been unlocked.
 */
void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (!instance || !instance->gameState) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // If the cursor is unlocked and awaiting a re-lock, a left-click will re-engage cursor lock.
        if (instance->gameState->awaitingRelock && !instance->gameState->cursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            instance->gameState->cursorLocked = true;
            instance->gameState->awaitingRelock = false;
            instance->gameState->firstMouse = true; // Reset to prevent camera jump on re-lock.
        }
        else if (instance->gameState->cursorLocked) {
            // Placeholder for future actions like shooting or attacking.
        }
    }
}

/**
 * @brief GLFW callback for mouse scroll wheel events.
 *
 * Passes the scroll offset to the camera to handle zooming.
 */
void InputHandler::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (instance && instance->gameState) {
        instance->gameState->camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

/**
 * @brief Processes continuous key presses each frame (polling).
 * @param window The active GLFW window.
 */
void InputHandler::processInput(GLFWwindow* window) {
    // Pressing ESCAPE unlocks the cursor and flags it to be re-locked on the next click.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && gameState->cursorLocked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        gameState->cursorLocked = false;
        gameState->awaitingRelock = true;
    }

    // Handle camera movement via WASD keys.
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(FORWARD, gameState->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(BACKWARD, gameState->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(LEFT, gameState->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(RIGHT, gameState->deltaTime);
}

/**
 * @brief Registers all static callback functions with GLFW.
 * @param window The window to attach the callbacks to.
 */
void InputHandler::setupCallbacks(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
}