#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderer.h"
#include "config.h"
#include <iostream>
#include <string>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(GameState* state) 
    : gameState(state), 
      levelShader(nullptr), 
      torchShader(nullptr), 
      swordShader(nullptr),
      level(nullptr), 
      torch(nullptr), 
      sword(nullptr) 
{
}

Renderer::~Renderer() {
    delete levelShader;
    delete torchShader;
    delete swordShader;
    delete level;
    delete torch;
    delete sword;
}

bool Renderer::initializeShaders() {
    try {
        levelShader = new Shader("shaders/levelVs.glsl", "shaders/levelFs.glsl");
        torchShader = new Shader("shaders/torchVs.glsl", "shaders/torchFs.glsl");
        swordShader = new Shader("shaders/swordVs.glsl", "shaders/swordFs.glsl");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize shaders: " << e.what() << std::endl;
        return false;
    }
}

bool Renderer::loadModels() {
    try {
        level = new Model("models/level/level.obj");
        torch = new Model("models/torch/torch.obj");
        sword = new Model("models/sword/sword.obj");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load models: " << e.what() << std::endl;
        return false;
    }
}

void Renderer::setupLighting(Shader& shader, float time) {
    shader.use();

    // Set view position
    shader.setVec3("viewPos", gameState->camera.Position);

    // Directional light (minimal - just basic fill light)
    shader.setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    shader.setVec3("dirLight.ambient", 0.005f, 0.005f, 0.01f);
    shader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.08f);
    shader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);

    // Point lights with flicker
    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        std::string number = std::to_string(i);
        glm::vec3 lightPos = POINT_LIGHT_POSITIONS[i];
        shader.setVec3("pointLights[" + number + "].position", lightPos);

        shader.setVec3("pointLights[" + number + "].ambient", 0.02f, 0.015f, 0.005f);

        // Base torch color
        glm::vec3 baseDiffuse(1.2f, 0.6f, 0.15f);

        // Flicker factor (each torch flickers slightly differently using offset)
        float flicker = 0.85f + 0.15f * sin(time * 7.0f + i * 1.3f) 
                              * cos(time * 5.0f + i * 2.1f);

        shader.setVec3("pointLights[" + number + "].diffuse", baseDiffuse * flicker);
        shader.setVec3("pointLights[" + number + "].specular", 0.1f, 0.1f, 0.05f);

        shader.setFloat("pointLights[" + number + "].constant", 1.0f);
        shader.setFloat("pointLights[" + number + "].linear", 0.25f);
        shader.setFloat("pointLights[" + number + "].quadratic", 0.25f);
    }

    shader.setFloat("material.shininess", 4.0f);

    shader.setFloat("fogNear", 4.0f);
    shader.setFloat("fogFar", 7.0f);
    shader.setVec3("fogColor", 0.02f, 0.02f, 0.04f);
}

void Renderer::setupTorchLighting(Shader& shader, float time) {
    shader.use();
    
    // Set view position
    shader.setVec3("viewPos", gameState->camera.Position);
    
    // Minimal directional light for torches
    shader.setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    shader.setVec3("dirLight.ambient", 0.01f, 0.005f, 0.002f);
    shader.setVec3("dirLight.diffuse", 0.1f, 0.08f, 0.05f);
    shader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
    
    // Material properties for torch
    shader.setFloat("material.shininess", 2.0f);
    shader.setFloat("material.emissiveStrength", 1.5f); // How much the torch glows
    shader.setFloat("time", time); // For flickering effect
    
    // Same fog settings as main scene
    shader.setFloat("fogNear", 4.0f);
    shader.setFloat("fogFar", 7.0f);
    shader.setVec3("fogColor", 0.02f, 0.02f, 0.04f);
}

void Renderer::renderLevel() {
    if (!levelShader || !level) return;
    
    setupLighting(*levelShader, static_cast<float>(glfwGetTime()));

    // Set up matrices for level
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
    
    glm::mat4 view = gameState->camera.GetViewMatrix();
    
    levelShader->setMat4("model", model);
    levelShader->setMat4("view", view);
    levelShader->setMat4("projection", gameState->projection);

    // Render the level
    level->Draw(*levelShader);
}

void Renderer::renderTorches() {
    if (!torchShader || !torch) return;
    
    setupTorchLighting(*torchShader, static_cast<float>(glfwGetTime()));
    
    glm::mat4 view = gameState->camera.GetViewMatrix();

    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        glm::mat4 torchModel = glm::mat4(1.0f);
        torchModel = glm::translate(torchModel, POINT_LIGHT_POSITIONS[i]);
        torchModel = glm::scale(torchModel, glm::vec3(1.0f, 1.0f, 1.0f));
        
        // Apply rotations based on position
        if (POINT_LIGHT_POSITIONS[i].z < -1.0f) {
            torchModel = glm::rotate(torchModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        } else if (POINT_LIGHT_POSITIONS[i].z > 1.0f) {
            torchModel = glm::rotate(torchModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        } else if (POINT_LIGHT_POSITIONS[i].x < -1.0f) {
            torchModel = glm::rotate(torchModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        torchShader->setMat4("model", torchModel);
        torchShader->setMat4("view", view);
        torchShader->setMat4("projection", gameState->projection);
        
        torch->Draw(*torchShader);
    }
}

void Renderer::renderSword() {
    if (!swordShader || !sword) return;
    
    setupLighting(*swordShader, static_cast<float>(glfwGetTime()));
    
    // Clear depth buffer for sword to render on top
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 swordModel = glm::mat4(1.0f);
    
    // Position sword relative to camera with fixed offsets
    glm::vec3 swordPos = gameState->camera.Position + 
                        gameState->camera.Front * 0.6f +     // Fixed distance forward
                        gameState->camera.Right * 0.6f -     // Fixed distance to the right  
                        gameState->camera.Up * 0.25f;        // Fixed distance down
    
    // Apply only subtle bobbing effect
    float swordBobOffset = sin(gameState->bobTimer) * (BOB_AMOUNT * 0.2f); // 20% of camera bob
    swordPos.y += swordBobOffset;
    
    swordModel = glm::translate(swordModel, swordPos);
    
    // Follow camera yaw rotation but keep pitch and roll static for stability
    float cameraYaw = atan2(gameState->camera.Front.x, gameState->camera.Front.z);
    float cameraPitch = asin(-gameState->camera.Front.y);
    swordModel = glm::rotate(swordModel, cameraYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    float limitedPitch = glm::clamp(cameraPitch * 0.8f, glm::radians(-70.0f), glm::radians(70.0f));
    swordModel = glm::rotate(swordModel, limitedPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Apply fixed sword orientation relative to camera direction
    swordModel = glm::rotate(swordModel, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Slight downward tilt
    swordModel = glm::rotate(swordModel, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Angle to the right
    swordModel = glm::rotate(swordModel, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // Slight roll
    
    glm::mat4 view = gameState->camera.GetViewMatrix();
    
    swordShader->setMat4("model", swordModel);
    swordShader->setMat4("view", view);
    swordShader->setMat4("projection", gameState->projection);
    
    sword->Draw(*swordShader);
}

void Renderer::render() {
    // Clear buffers with a very dark PS1-style background
    glClearColor(0.01f, 0.01f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update projection matrix
    gameState->projection = glm::perspective(
        glm::radians(gameState->camera.Zoom), 
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 
        0.1f, 100.0f
    );

    // Render all objects
    renderLevel();
    renderTorches();
    renderSword();
}