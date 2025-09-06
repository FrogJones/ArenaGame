/**
 * @file gameEngine.cpp
 * @brief Main orchestrator for the game, managing initialization, the game loop, and cleanup.
 *
 * This file contains the core logic that brings all the subsystems (renderer, input, audio, UI)
 * together. It's responsible for the game's lifecycle from start to finish.
 */

#include "gameEngine.h"
#include "config.h"
#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>

// Define STB_IMAGE_IMPLEMENTATION in one and only one CPP file to create the implementation.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>

/**
 * @brief Constructs the GameEngine, initializing all subsystem pointers to null.
 */
GameEngine::GameEngine() 
    : window(nullptr),
      inputHandler(nullptr),
      renderer(nullptr),
      audioManager(nullptr),
      gui(nullptr),
      ambianceBuffer(0),
      ambianceSource(0)
{
    // Seed the random number generator once for application-wide use.
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

/**
 * @brief Destroys the GameEngine, ensuring all resources are cleaned up.
 */
GameEngine::~GameEngine() {
    cleanup();
}

/**
 * @brief Initializes all game subsystems in the correct order.
 * @return True if all subsystems initialize successfully, false otherwise.
 */
bool GameEngine::initialize() {
    // Initialize core libraries (GLFW, GLAD) and create the window.
    window = initializeGLFW();
    if (!window) {
        std::cerr << "FATAL: Failed to initialize GLFW window" << std::endl;
        return false;
    }

    if (!initializeGLAD()) {
        std::cerr << "FATAL: Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Configure global OpenGL state.
    glEnable(GL_DEPTH_TEST);
    
    // Set texture filtering to nearest-neighbor for a retro, pixelated aesthetic.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Most 3D models are authored with textures that expect Y-up, but OpenGL's
    // texture coordinates are Y-down. This flips them to match.
    stbi_set_flip_vertically_on_load(true);

    // Initialize game-specific managers.
    inputHandler = new InputHandler(&gameState);
    inputHandler->setupCallbacks(window);

    renderer = new Renderer(&gameState);
    if (!renderer->initializeShaders() || !renderer->loadModels()) {
        std::cerr << "FATAL: Failed to initialize renderer" << std::endl;
        return false;
    }

    if (!initializeAudio() || !loadAudioAssets()) {
        std::cerr << "FATAL: Failed to initialize audio systems" << std::endl;
        return false;
    }

    gui = new GUI();
    if (!gui->Initialize(window)) {
        std::cerr << "FATAL: Failed to initialize GUI" << std::endl;
        return false;
    };

    // Define all in-game interactive objects.
    setupGameInteractions();

    std::cout << "Game engine initialized successfully!" << std::endl;
    return true;
}

/**
 * @brief The main game loop. Runs continuously until the window is closed.
 */
void GameEngine::run() {
    while (!glfwWindowShouldClose(window)) {
        // 1. Update timing and process user input.
        gameState.updateTiming();
        inputHandler->processInput(window);
        
        // 2. Update game state (player movement, interactions).
        gameState.updateMovement();
        gameState.updateInteraction(window);
        gameState.updatePopup();
        
        // 3. Update systems that depend on game state (e.g., audio).
        handleMovementAudio();
        
        // 4. Render the scene and UI.
        renderer->render();
        gui->NewFrame();
        gui->Render(&gameState);

        // 5. Swap buffers to display the new frame and poll for events.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

/**
 * @brief Releases all allocated resources in reverse order of initialization.
 */
void GameEngine::cleanup() {
    // Safely delete all heap-allocated managers.
    if (gui) {
        gui->Shutdown();
        delete gui;
        gui = nullptr;
    }
    if (inputHandler) {
        delete inputHandler;
        inputHandler = nullptr;
    }
    if (renderer) {
        delete renderer;
        renderer = nullptr;
    }
    if (audioManager) {
        delete audioManager;
        audioManager = nullptr;
    }
 
    // Terminate GLFW after all other resources are freed.
    if (window) {
        glfwTerminate();  
        window = nullptr;
    }
}

/**
 * @brief Initializes GLFW and creates the main application window.
 * @return A pointer to the created GLFWwindow, or nullptr on failure.
 */
GLFWwindow* GameEngine::initializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Request a specific OpenGL version (e.g., 3.3 Core Profile).
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* newWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PS1 Level Viewer", nullptr, nullptr);
    if (!newWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(newWindow);
    // Lock the cursor to the window for first-person camera control.
    glfwSetInputMode(newWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return newWindow;
}

/**
 * @brief Initializes GLAD, the OpenGL function loader.
 * @return True on success, false on failure.
 */
bool GameEngine::initializeGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Initializes the core audio manager.
 * @return True on success, false on failure.
 */
bool GameEngine::initializeAudio() {
    audioManager = new AudioManager();
    if (!audioManager->init()) {
        std::cerr << "Failed to initialize AudioManager" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Loads all necessary audio files into memory.
 * @return True on success, false if critical assets are missing.
 */
bool GameEngine::loadAudioAssets() {
    // Load a variety of step sounds for random playback.
    for (int i = 1; i < 4; i++) {
        std::string filename = "sfx/steps/step" + std::to_string(i) + ".wav";
        ALuint buffer = audioManager->loadAudio(filename);
        if (buffer != 0) {
            stepSounds.push_back(buffer);
        }
    }

    if (stepSounds.empty()) {
        std::cerr << "Warning: No step sounds were loaded. Movement will be silent." << std::endl;
    }

    // Load and play the looping ambient background sound.
    ambianceBuffer = audioManager->loadAudio("sfx/env/ambiance.wav");
    if (ambianceBuffer != 0) {
        ambianceSource = audioManager->playSound(ambianceBuffer, true); // Loop enabled.
        if (ambianceSource != 0) {
            alSourcef(ambianceSource, AL_GAIN, 0.2f); // Set a lower volume for ambiance.
            // Make the sound non-positional so it's always heard at the same volume.
            alSourcei(ambianceSource, AL_SOURCE_RELATIVE, AL_TRUE);
            alListenerf(AL_GAIN, 1.0f); // Ensure listener gain is at default.
        }
    } else {
        std::cerr << "Warning: Failed to load ambiance.wav. The world will be eerily quiet." << std::endl;
    }

    return true;
}

/**
 * @brief Plays a random step sound if the player has moved and the cooldown has expired.
 */
void GameEngine::handleMovementAudio() {
    float moveDistance = glm::length(gameState.camera.Position - gameState.lastCameraPos);
    
    // Check if the player has moved a significant distance and the sound cooldown is over.
    if (moveDistance > MOVEMENT_THRESHOLD && gameState.stepCooldown <= 0.0f && !stepSounds.empty()) {
        // Pick and play a random step sound from the loaded assets.
        int idx = std::rand() % static_cast<int>(stepSounds.size());
        audioManager->playSound(stepSounds[idx]);
        
        // Reset the cooldown to prevent sound spam.
        gameState.stepCooldown = STEP_COOLDOWN;
    }
}

/**
 * @brief Defines all interactive objects in the game world.
 *
 * This function serves as a centralized hub for creating and configuring all
 * interactable entities, such as pickups, doors, and chests.
 */
void GameEngine::setupGameInteractions() {
    // Creates the interactable object for the broken sword at the bonfire.
    gameState.interactionSystem.AddInteractable(
        glm::vec3(0.0f, 1.0f, 0.0f),
        "E - Pull out.", "Broken sword acquired.",
        [this]() {
            // This lambda function is executed when the player interacts.
            gameState.hasBrokenSword = true;
            gameState.swordType = "broken";
            std::cout << "Interaction: Broken sword picked up!" << std::endl;
        }
    );

    // Future interactions (chests, doors, NPCs) can be added here.
}
