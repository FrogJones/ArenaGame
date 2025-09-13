/**
 * @file GUI.cpp
 * @brief Implementation of the graphical user interface using ImGui.
 *
 * This file handles the initialization, rendering, and shutdown of all UI elements,
 * including menus, crosshairs, and in-game prompts.
 */

#include "GUI.h"
#include "stb_image.h"

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
    
    // Example: Regular text buttons
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
        ImGui::Text("Inventory is empty");
    } else {
        ImGui::Text("Inventory:");
        ImGui::Separator();
        
        // Display items in a grid layout
        int itemsPerRow = 10;
        int itemCount = 0;
        
        for (const auto& item : items) {
            // Get texture for this item (you can map item names to texture paths)
            std::string texturePath = item.getImagePath();
            GLuint textureID = LoadImageTexture(texturePath);
            
            if (textureID != 0) {
                // Push custom styles for image buttons
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));        // Normal state
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.1f, 1.0f)); // Hover state (golden)
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.2f, 1.0f));  // Click state (bright golden)
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);                         // Rounded corners
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));                  // Internal padding
                
                // Create image button
                ImVec2 buttonSize(64, 64);
                if (ImGui::ImageButton(("##" + item.getName()).c_str(), 
                                     textureID, 
                                     buttonSize, ImVec2(0,1), ImVec2(1,0))) {
                    // Toggle description display
                    if (gameState->showItemDescription && gameState->selectedItemDescription == item.getDescription()) {
                        // Hide if clicking the same item
                        gameState->showItemDescription = false;
                        gameState->selectedItemDescription = "";
                    } else {
                        // Show description for this item
                        gameState->showItemDescription = true;
                        gameState->selectedItemDescription = item.getDescription();
                    }
                }
                
                // Pop the custom styles
                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(3);
                
                // Tooltip on hover
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", item.getName().c_str());
                    ImGui::EndTooltip();
                }
            } else {
                // Fallback to text button if no texture
                if (ImGui::Button(item.getName().c_str(), ImVec2(64, 64))) {
                }
            }
            
            // Move to next column or row
            itemCount++;
            if (itemCount % itemsPerRow != 0) {
                ImGui::SameLine();
            }
        }
    }

    ImGui::End();
    
    // Don't forget to pop the styles
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}

/**
 * @brief Renders the item description in the center of the screen when an item is selected.
 * @param gameState The current game state, containing selected item description.
 */
void GUI::RenderItemDescription(GameState* gameState) {
    if (!gameState->showItemDescription || gameState->selectedItemDescription.empty()) return;
   
    ImGuiIO& io = ImGui::GetIO();
    // Position the description in the center of the screen.
    float centerX = io.DisplaySize.x * 0.5f;
    float centerY = io.DisplaySize.y * 0.5f;
   
    // Use a styled window to display the item description.
    ImGui::SetNextWindowPos(ImVec2(centerX, centerY), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // Centered pivot.
    ImGui::SetNextWindowBgAlpha(0.9f); // Semi-transparent background.
    
    // Push custom colors for the description window
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f)); // Dark background
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.8f, 0.2f, 1.0f)); // Yellow border
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.8f, 1.0f)); // Light yellow text
    
    // Custom padding and rounding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
    
    ImGui::Begin("ItemDescription", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize
    );

    // Scale the text to be larger for better readability
    ImGui::SetWindowFontScale(1.2f);
    
    // Render the item description text.
    ImGui::Text("%s", gameState->selectedItemDescription.c_str());
    
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);

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
    if (gameState->showItemDescription) {
        RenderItemDescription(gameState);
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
 * @brief Loads an image as an OpenGL texture for use with ImGui ImageButton.
 * @param imagePath Path to the image file to load.
 * @return OpenGL texture ID, or 0 if loading failed.
 */
GLuint GUI::LoadImageTexture(const std::string& imagePath) {
    // Check if texture is already loaded
    auto it = imageTextures.find(imagePath);
    if (it != imageTextures.end()) {
        return it->second;
    }

    GLuint textureID = 0;
    int width, height, channels;
    
    // Load image data using stb_image
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        return 0;
    }

    // Generate OpenGL texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload texture data
    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Free image data
    stbi_image_free(data);

    // Cache the texture
    imageTextures[imagePath] = textureID;

    std::cout << "Loaded texture: " << imagePath << " (ID: " << textureID << ")" << std::endl;
    return textureID;
}

/**
 * @brief Frees all loaded image textures.
 */
void GUI::FreeImageTextures() {
    for (auto& pair : imageTextures) {
        glDeleteTextures(1, &pair.second);
    }
    imageTextures.clear();
}

/**
 * @brief Shuts down ImGui and releases its resources.
 */
void GUI::Shutdown() {
    FreeImageTextures(); // Free loaded image textures
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}