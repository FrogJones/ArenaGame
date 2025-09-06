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
    Shader* bonfireShader;
    Shader* swordShader;
    
    // Models
    Model* level;
    Model* bonfireSword;
    Model* bonfire;
    Model* sword;
    Model* brokenSword;
    Model* lightBeam;
    
    GameState* gameState;
    
public:
    Renderer(GameState* state);
    ~Renderer();
    
    bool initializeShaders();
    bool loadModels();
    
    void setupLighting(Shader& shader, float time);
    void setupTorchLighting(Shader& shader, float time);
    
    void renderLevel();
    void renderSword(std::string type);
    void renderBonfire(bool hasBrokenSword);
    void renderLightBeam();
    void render();
};

#endif