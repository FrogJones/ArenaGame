#include "gameEngine.h"
#include "config.h"
#include <iostream>
#include <random>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GameEngine::GameEngine() 
    : window(nullptr),
      inputHandler(nullptr),
      renderer(nullptr),
      audioManager(nullptr),
      ambianceBuffer(0),
      ambianceSource(0)
{
}

GameEngine::~GameEngine() {
    cleanup();
}

bool GameEngine::initialize() {
    // Initialize GLFW and create window
    window = initializeGLFW();
    if (!window) {
        std::cerr << "Failed to initialize GLFW window" << std::endl;
        return false;
    }

    // Initialize GLAD
    if (!initializeGLAD()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);
    
    // Set texture filtering to nearest for PS1 pixelated look
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Flip textures vertically on load (for models)
    stbi_set_flip_vertically_on_load(true);

    // Initialize input handler
    inputHandler = new InputHandler(&gameState);
    inputHandler->setupCallbacks(window);

    // Initialize renderer
    renderer = new Renderer(&gameState);
    if (!renderer->initializeShaders()) {
        std::cerr << "Failed to initialize shaders" << std::endl;
        return false;
    }
    if (!renderer->loadModels()) {
        std::cerr << "Failed to load models" << std::endl;
        return false;
    }

    // Initialize audio
    if (!initializeAudio()) {
        std::cerr << "Failed to initialize audio" << std::endl;
        return false;
    }

    if (!loadAudioAssets()) {
        std::cerr << "Failed to load audio assets" << std::endl;
        return false;
    }

    std::cout << "Game engine initialized successfully!" << std::endl;
    return true;
}

void GameEngine::run() {
    while (!glfwWindowShouldClose(window)) {
        // Update timing
        gameState.updateTiming();
        
        // Process input
        inputHandler->processInput(window);
        
        // Update camera movement and boundaries
        gameState.updateMovement();
        
        // Handle movement-based audio
        handleMovementAudio();
        
        // Render frame
        renderer->render();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void GameEngine::cleanup() {
    delete inputHandler;
    delete renderer;
    delete audioManager;
    
    if (window) {
        glfwTerminate();
    }
}

GLFWwindow* GameEngine::initializeGLFW() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PS1 Level Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

bool GameEngine::initializeGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

bool GameEngine::initializeAudio() {
    audioManager = new AudioManager();
    if (!audioManager->init()) {
        std::cerr << "Failed to initialize AudioManager" << std::endl;
        return false;
    }
    return true;
}

bool GameEngine::loadAudioAssets() {
    // Load step sounds
    for (int i = 1; i < 4; i++) {
        std::string filename = "sfx/steps/step" + std::to_string(i) + ".wav";
        ALuint buffer = audioManager->loadAudio(filename);
        if (buffer != 0) {
            stepSounds.push_back(buffer);
        }
    }

    if (stepSounds.empty()) {
        std::cerr << "Warning: No step sounds loaded" << std::endl;
    }

    // Load and play ambiance
    ambianceBuffer = audioManager->loadAudio("sfx/env/ambiance.wav");
    if (ambianceBuffer != 0) {
        ambianceSource = audioManager->playSound(ambianceBuffer, true); // 'true' enables looping
        alSourcef(ambianceSource, AL_GAIN, 0.2f); // Lower volume for ambiance
    }

    return true;
}

void GameEngine::handleMovementAudio() {
    // Calculate movement distance
    float moveDistance = glm::length(gameState.camera.Position - gameState.lastCameraPos);
    
    // Threshold to avoid playing sound for very tiny movements
    if (moveDistance > MOVEMENT_THRESHOLD && gameState.stepCooldown <= 0.0f && !stepSounds.empty()) {
        // Pick a random step sound
        int idx = rand() % stepSounds.size();
        audioManager->playSound(stepSounds[idx]);
        
        gameState.stepCooldown = STEP_COOLDOWN; // Reset cooldown
    }
}