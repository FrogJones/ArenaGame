#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "gameState.h"
#include <iostream>


class GUI {
private:
    // No more UI state flags - moved to GameState
    
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
    void ToggleMenu(GameState* gameState);
    
    bool IsMenuOpen(GameState* gameState) const { return gameState->showMenu; }
};