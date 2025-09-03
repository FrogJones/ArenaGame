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
    bool showCrosshair = true;
    bool showMenu = false;
    
public:
    bool Initialize(GLFWwindow* window); 
    void NewFrame();                      
    void Render(GameState* gameState);                        
    void Shutdown();                      
    
    void RenderCrosshair();
    void RenderInteractionPrompt(GameState* gameState);
    void RenderMenu();
    void ToggleMenu();
    
    bool IsMenuOpen() const { return showMenu; }
};