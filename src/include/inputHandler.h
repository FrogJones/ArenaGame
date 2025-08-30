#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gameState.h"

class InputHandler {
private:
    GameState* gameState;
    
public:
    InputHandler(GameState* state);
    
    // Callback functions (static)
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    
    // Input processing
    void processInput(GLFWwindow* window);
    void setupCallbacks(GLFWwindow* window);
    
    // Static instance for callbacks
    static InputHandler* instance;
};

#endif