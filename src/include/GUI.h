#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "gameState.h"
#include <iostream>
#include <unordered_map>
#include <string>

class GUI {
private:
    // No more UI state flags - moved to GameState
    std::unordered_map<std::string, GLuint> imageTextures; // Cache for loaded textures
    
public:
    bool Initialize(GLFWwindow* window); 
    void NewFrame();                      
    void Render(GameState* gameState);                        
    void Shutdown();                      
    
    void RenderCrosshair(GameState* gameState);
    void RenderInteractionPrompt(GameState* gameState);
    void RenderPopup(GameState* gameState);
    void RenderMenu(GameState* gameState);
    void RenderInventory(GameState* gameState);
    void RenderItemDescription(GameState* gameState);
    void ToggleMenu(GameState* gameState);
    
    // Image button utilities
    GLuint LoadImageTexture(const std::string& imagePath);
    void FreeImageTextures();
    
    bool IsMenuOpen(GameState* gameState) const { return gameState->showMenu; }
};