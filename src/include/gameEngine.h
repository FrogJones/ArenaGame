#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <audioManager.h>

#include "gameState.h"
#include "inputHandler.h"
#include "renderer.h"
#include "GUI.h"
#include "interactionSystem.h"

class GameEngine {
private:
    GLFWwindow* window;
    GameState gameState;
    InputHandler* inputHandler;
    Renderer* renderer;
    AudioManager* audioManager;
    GUI* gui;
    
    // Audio
    std::vector<ALuint> stepSounds;
    ALuint ambianceBuffer;
    ALuint ambianceSource;
    
public:
    GameEngine();
    ~GameEngine();
    
    bool initialize();
    void run();
    void cleanup();
    
private:
    GLFWwindow* initializeGLFW();
    bool initializeGLAD();
    bool initializeAudio();
    bool loadAudioAssets();
    void setupGameInteractions();
    
    void handleMovementAudio();
};


#endif