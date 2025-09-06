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
void GUI::RenderCrosshair(GameState* gameState) {
    if (!gameState->showCrosshair) return;

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
void GUI::RenderMenu(GameState* gameState) {
    if (!gameState->showMenu) return;
    
    ImGui::Begin("Game Menu");
    
    if (ImGui::Button("Resume")) {
        gameState->showMenu = false;
    }
    if (ImGui::Button("Options")) {
        // Placeholder for options logic.
    }
    if (ImGui::Button("Quit")) {
        // Placeholder for quit logic.
    }
    
    ImGui::End();
}

void GUI::RenderInventory(GameState* gameState) {
    // Push custom colors
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f)); // Dark background
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.8f, 0.2f, 1.0f)); // Yellow border
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.8f, 1.0f)); // Light yellow text
    
    // Custom padding and rounding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    
    ImGui::Begin("Inventory", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
    
    const auto& items = gameState->inventory.getItems();
    if (items.empty()) {
        ImGui::Text("Inventory is empty.");
    } else {
        for (const auto& item : items) {
            ImGui::Text("%s:\n", item.getName().c_str());
            ImGui::Text("%s", item.getDescription().c_str());
        }
    }

    ImGui::End();
    
    // Don't forget to pop the styles
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
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
    ImGui::SetNextWindowBgAlpha(0.5f); // semi-transparent background.
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.8f, 1.0f)); // Light yellow text
    ImGui::Begin("InteractionPrompt", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize
    );

    // Render the interaction text.
    ImGui::Text("%s", gameState->interactionText.c_str());
    ImGui::PopStyleColor(1);

    ImGui::End();
}

/**
 * @brief Renders popup messages when interactions occur at the bottom-left corner.
 * @param gameState The current game state, containing popup text.
 */
void GUI::RenderPopup(GameState* gameState) {
    if (gameState->interactionPopup.empty()) return;

    ImGuiIO& io = ImGui::GetIO();
    // Position the popup at the bottom-left corner.
    float posX = io.DisplaySize.x * 0.02f;
    float posY = io.DisplaySize.y * 0.95f;
    
    ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Always, ImVec2(0.0f, 1.0f)); // Bottom-left pivot.
    ImGui::SetNextWindowBgAlpha(0.8f); // Semi-transparent background for visibility.
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.8f, 1.0f)); // Light yellow text
    ImGui::Begin("Popup", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoBackground
    );

    // Scale the text to be larger for better visibility
    ImGui::SetWindowFontScale(1.5f); // 1.5x larger text
    ImGui::Text("%s", gameState->interactionPopup.c_str());
    ImGui::PopStyleColor(1);
    
    ImGui::End();
}

/**
 * @brief Main render call for the GUI. Determines which UI components to draw.
 * @param gameState The current game state, passed to components that need it.
 */
void GUI::Render(GameState* gameState) {
    if (gameState->showMenu) {
        RenderMenu(gameState);
    } else {
        if (gameState->showCrosshair) {
            RenderCrosshair(gameState);
        }
        RenderInteractionPrompt(gameState);
        RenderPopup(gameState);
    }
    if (gameState->showInventory) {
        RenderInventory(gameState);
    }
    
    // Finalize the ImGui frame and render its draw data.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * @brief Toggles the visibility of the main menu.
 */
void GUI::ToggleMenu(GameState* gameState) {
    gameState->showMenu = !gameState->showMenu;
}

/**
 * @brief Shuts down ImGui and releases its resources.
 */
void GUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}