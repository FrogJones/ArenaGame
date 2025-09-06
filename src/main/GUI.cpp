/**
 * @file GUI.cpp
 * @brief Implementation of the graphical user interface using ImGui.
 *
 * This file handles the initialization, rendering, and shutdown of all UI elements,
 * including menus, crosshairs, and in-game prompts.
 */

#include "GUI.h"

/**
 * @brief Initializes ImGui, its backends (GLFW, OpenGL3), and loads custom fonts.
 * @param window The main GLFW window.
 * @return True on successful initialization, false otherwise.
 */
bool GUI::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Optional: Enable keyboard/gamepad navigation.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Load a custom font for a stylized look. Fallback to default if it fails.
    ImFont* custom = io.Fonts->AddFontFromFileTTF("fonts/MorrisRoman-Black.ttf", 18.0f);
    if (custom) {
        io.FontDefault = custom;
    } else {
        std::cerr << "Warning: Failed to load custom font. Using ImGui default." << std::endl;
    }

    // Initialize the platform and renderer backends for ImGui.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // The font texture must be created after the OpenGL context is initialized.
    ImGui_ImplOpenGL3_CreateFontsTexture();

    return true;
}

/**
 * @brief Starts a new ImGui frame, preparing it for rendering commands.
 */
void GUI::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/**
 * @brief Renders a simple dot crosshair in the center of the screen.
 *
 * Uses the foreground draw list to ensure it's rendered on top of all other UI elements.
 */
void GUI::RenderCrosshair() {
    if (!showCrosshair) return;

    // Calculate the center of the viewport.
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 center(displaySize.x * 0.5f, displaySize.y * 0.5f);

    // Define crosshair appearance.
    float radius = 3.0f;
    ImU32 color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White
    ImU32 outlineColor = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.5f)); // Semi-transparent black

    // Draw the crosshair using ImGui's drawing API.
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddCircleFilled(center, radius, color);
    drawList->AddCircle(center, radius + 0.8f, outlineColor, 16, 1.0f); // Outline for visibility.
}

/**
 * @brief Renders the main game menu.
 */
void GUI::RenderMenu() {
    if (!showMenu) return;
    
    ImGui::Begin("Game Menu");
    
    if (ImGui::Button("Resume")) {
        showMenu = false;
    }
    if (ImGui::Button("Options")) {
        // Placeholder for options logic.
    }
    if (ImGui::Button("Quit")) {
        // Placeholder for quit logic.
    }
    
    ImGui::End();
}

/**
 * @brief Renders the interaction prompt (e.g., "E - Pick up") when the player is near an object.
 * @param gameState The current game state, containing interaction text and status.
 */
void GUI::RenderInteractionPrompt(GameState* gameState) {
    if (!gameState->showInteractionPrompt || gameState->interactionText.empty()) return;
   
    ImGuiIO& io = ImGui::GetIO();
    // Position the prompt slightly below the center of the screen.
    float centerX = io.DisplaySize.x * 0.5f;
    float centerY = io.DisplaySize.y * 0.55f;
   
    // Use a transparent, borderless window to act as a container for the text.
    ImGui::SetNextWindowPos(ImVec2(centerX, centerY), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // Centered pivot.
    ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background.
    ImGui::Begin("InteractionPrompt", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize
    );

    // Render the interaction text.
    ImGui::Text("%s", gameState->interactionText.c_str());
   
    ImGui::End();
}

/**
 * @brief Main render call for the GUI. Determines which UI components to draw.
 * @param gameState The current game state, passed to components that need it.
 */
void GUI::Render(GameState* gameState) {
    if (showMenu) {
        RenderMenu();
    } else {
        if (showCrosshair) {
            RenderCrosshair();
        }
        RenderInteractionPrompt(gameState);
    }
    
    // Finalize the ImGui frame and render its draw data.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * @brief Toggles the visibility of the main menu.
 */
void GUI::ToggleMenu() {
    showMenu = !showMenu;
}

/**
 * @brief Shuts down ImGui and releases its resources.
 */
void GUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}