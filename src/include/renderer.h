#ifndef RENDERER_H
#define RENDERER_H

#include <shader.h>
#include <model.h>
#include <vector>
#include <AL/al.h>
#include "gameState.h"

class Renderer {
private:
    // Shaders
    Shader* levelShader;
    Shader* torchShader;
    Shader* swordShader;
    
    // Models
    Model* level;
    Model* torch;
    Model* sword;
    
    GameState* gameState;
    
public:
    Renderer(GameState* state);
    ~Renderer();
    
    bool initializeShaders();
    bool loadModels();
    
    void setupLighting(Shader& shader, float time);
    void setupTorchLighting(Shader& shader, float time);
    
    void renderLevel();
    void renderTorches();
    void renderSword();
    void render();
};

#endif