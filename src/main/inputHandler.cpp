#include "inputHandler.h"
#include "config.h"
#include <glm/gtc/matrix_transform.hpp>

// Static instance for callbacks
InputHandler* InputHandler::instance = nullptr;

InputHandler::InputHandler(GameState* state) : gameState(state) {
    instance = this;
}

void InputHandler::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    if (instance && instance->gameState) {
        float aspect = (float)width / (float)height;
        instance->gameState->projection = glm::perspective(
            glm::radians(instance->gameState->camera.Zoom), 
            aspect, 
            0.01f, 
            20.0f
        );
    }
}

void InputHandler::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (!instance || !instance->gameState || !instance->gameState->cursorLocked) {
        return; // ignore mouse movement when cursor is unlocked
    }
    
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (instance->gameState->firstMouse) {
        instance->gameState->lastX = xpos;
        instance->gameState->lastY = ypos;
        instance->gameState->firstMouse = false;
    }

    float xoffset = xpos - instance->gameState->lastX;
    float yoffset = instance->gameState->lastY - ypos; // Reversed since y-coordinates go from bottom to top
    
    instance->gameState->lastX = xpos;
    instance->gameState->lastY = ypos;

    instance->gameState->camera.ProcessMouseMovement(xoffset, yoffset);
}

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (!instance || !instance->gameState) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (instance->gameState->awaitingRelock && !instance->gameState->cursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // relock
            instance->gameState->cursorLocked = true;
            instance->gameState->awaitingRelock = false;
            instance->gameState->firstMouse = true; // reset mouse delta to avoid jump
        }
        else if (instance->gameState->cursorLocked) {
            // Normal left click action (shoot/attack/etc.)
            // Add your game actions here
        }
    }
}

void InputHandler::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (instance && instance->gameState) {
        instance->gameState->camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

void InputHandler::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && gameState->cursorLocked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // unlock
        gameState->cursorLocked = false;
        gameState->awaitingRelock = true; // wait for first click
    }

    // Camera movement using WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(FORWARD, gameState->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(BACKWARD, gameState->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(LEFT, gameState->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gameState->camera.ProcessKeyboard(RIGHT, gameState->deltaTime);
    
    // Uncomment these if you want vertical movement
    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    //     gameState->camera.ProcessKeyboard(UP, gameState->deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    //     gameState->camera.ProcessKeyboard(DOWN, gameState->deltaTime);
}

void InputHandler::setupCallbacks(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
}