#include "GUI.h"

bool GUI::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Enable keyboard/gamepad navigation if needed
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Optional: scale UI for high-DPI displays
    // io.FontGlobalScale = 1.0f;

    ImFont* custom = io.Fonts->AddFontFromFileTTF("fonts/MorrisRoman-Black.ttf", 18.0f);
    if (custom) {
        io.FontDefault = custom; // make it the default font
    } else {
        std::cerr << "Failed to load font: resources/fonts/YourFont-Regular.ttf\n";
    }

    // Initialize platform/renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Force font texture upload now
    ImGui_ImplOpenGL3_CreateFontsTexture();

    return true;
}

void GUI::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::RenderCrosshair() {
    if (!showCrosshair) return;

    // center of the screen
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 center(displaySize.x * 0.5f, displaySize.y * 0.5f);

    // choose size and color
    float radius = 3.0f;            // increase for a bigger dot
    ImU32 color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // white

    // draw on top via overlay draw list
    ImDrawList* draw = ImGui::GetForegroundDrawList();
    // filled circle (dot)
    draw->AddCircleFilled(center, radius, color);

    // optional subtle outline to make it pop on dark/light backgrounds
    ImU32 outline = ImGui::GetColorU32(ImVec4(0,0,0,0.5f));
    draw->AddCircle(center, radius + 0.8f, outline, 16, 1.0f);
}

void GUI::RenderMenu() {
    if (!showMenu) return;
    
    ImGui::Begin("Game Menu");
    
    if (ImGui::Button("Resume")) {
        showMenu = false;
    }
    
    if (ImGui::Button("Options")) {
        // Handle options
    }
    
    if (ImGui::Button("Quit")) {
        // Handle quit
    }
    
    ImGui::End();
}

void GUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::ToggleMenu() {
    showMenu = !showMenu;
}
    
void GUI::Render(GameState* gameState) {
    if (showMenu) {
        RenderMenu();
    } else {
        if (showCrosshair) {
            RenderCrosshair();
        }
        
        // Show interaction prompt
        RenderInteractionPrompt(gameState);
    }
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::RenderInteractionPrompt(GameState* gameState) {
    if (!gameState->showInteractionPrompt) return;
   
    ImGuiIO& io = ImGui::GetIO();
    float centerX = io.DisplaySize.x * 0.5f;
    float centerY = io.DisplaySize.y * 0.6f;
   
    ImGui::SetNextWindowPos(ImVec2(centerX - 100, centerY));
    ImGui::SetNextWindowSize(ImVec2(200, 50));
    ImGui::Begin("Interaction", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoMove);
   
    // Simple centered text
    float textWidth = ImGui::CalcTextSize(gameState->interactionText.c_str()).x;
    ImGui::SetCursorPosX((200 - textWidth) * 0.5f);
    ImGui::Text("%s", gameState->interactionText.c_str());
   
    ImGui::End();
}