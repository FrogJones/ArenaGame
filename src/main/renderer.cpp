/**
 * @file renderer.cpp
 * @brief Implements the main rendering logic for the application.
 *
 * This file contains the implementation of the Renderer class, which is responsible
 * for all drawing operations. It manages shaders, models, lighting, and the
 * rendering of different game objects like the level, bonfire, and the player's sword.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderer.h"
#include "config.h"
#include <iostream>
#include <string>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Constructs the Renderer.
 * @param state A pointer to the shared GameState object.
 */
Renderer::Renderer(GameState* state) 
    : gameState(state), 
      levelShader(nullptr), 
      swordShader(nullptr),
      bonfireShader(nullptr),
      level(nullptr), 
      bonfire(nullptr),
      bonfireSword(nullptr),
      brokenSword(nullptr),
      sword(nullptr) 
{
}

/**
 * @brief Destroys the Renderer, cleaning up all allocated resources.
 */
Renderer::~Renderer() {
    delete levelShader;
    delete swordShader;
    delete bonfireShader;
    delete level;
    delete bonfireSword;
    delete bonfire;
    delete brokenSword;
    delete sword;
}

/**
 * @brief Initializes all shader programs used for rendering.
 * @return True if shaders were created successfully, false otherwise.
 */
bool Renderer::initializeShaders() {
    try {
        levelShader = new Shader("shaders/level/levelVs.glsl", "shaders/level/levelFs.glsl");
        swordShader = new Shader("shaders/sword/swordVs.glsl", "shaders/sword/swordFs.glsl");
        bonfireShader = new Shader("shaders/bonfire/bonfireVs.glsl", "shaders/bonfire/bonfireFs.glsl");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize shaders: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Loads all 3D models required for the scene.
 * @return True if models were loaded successfully, false otherwise.
 */
bool Renderer::loadModels() {
    try {
        level = new Model("models/level/level.obj");
        sword = new Model("models/sword/sword.obj");
        bonfireSword = new Model("models/bonfireSword/bonfire.obj");
        bonfire = new Model("models/bonfire/bonfire.obj");
        brokenSword = new Model("models/brokenSword/broken_sword.obj");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load models: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Configures the lighting and fog for the main scene shader.
 * @param shader The shader program to configure.
 * @param time The current application time for animations.
 */
void Renderer::setupLighting(Shader& shader, float time) {
    shader.use();

    shader.setVec3("viewPos", gameState->camera.Position);

    // Minimal directional light for basic ambient fill.
    shader.setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    shader.setVec3("dirLight.ambient", 0.005f, 0.005f, 0.01f);
    shader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.08f);
    shader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);

    // Configure multiple point lights to simulate torch flickers.
    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        std::string number = std::to_string(i);
        glm::vec3 lightPos = POINT_LIGHT_POSITIONS[i];
        shader.setVec3("pointLights[" + number + "].position", lightPos);
        shader.setVec3("pointLights[" + number + "].ambient", 0.02f, 0.015f, 0.005f);

        glm::vec3 baseDiffuse(1.2f, 0.6f, 0.15f);
        float flicker = 0.85f + 0.15f * sin(time * 7.0f + i * 1.3f) * cos(time * 5.0f + i * 2.1f);
        shader.setVec3("pointLights[" + number + "].diffuse", baseDiffuse * flicker);
        shader.setVec3("pointLights[" + number + "].specular", 0.1f, 0.1f, 0.05f);

        // Attenuation values for a realistic falloff.
        shader.setFloat("pointLights[" + number + "].constant", 1.0f);
        shader.setFloat("pointLights[" + number + "].linear", 0.25f);
        shader.setFloat("pointLights[" + number + "].quadratic", 0.25f);
    }

    shader.setFloat("material.shininess", 4.0f);

    // Fog settings for a dense, atmospheric effect.
    shader.setFloat("fogNear", 4.0f);
    shader.setFloat("fogFar", 7.0f);
    shader.setVec3("fogColor", 0.02f, 0.02f, 0.04f);
}

/**
 * @brief Configures a specialized lighting setup for emissive objects like torches.
 * @param shader The shader program to configure.
 * @param time The current application time for animations.
 */
void Renderer::setupTorchLighting(Shader& shader, float time) {
    shader.use();
    
    shader.setVec3("viewPos", gameState->camera.Position);
    
    shader.setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    shader.setVec3("dirLight.ambient", 0.01f, 0.005f, 0.002f);
    shader.setVec3("dirLight.diffuse", 0.1f, 0.08f, 0.05f);
    shader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
    
    shader.setFloat("material.shininess", 2.0f);
    shader.setFloat("material.emissiveStrength", 1.5f); // Controls glow intensity.
    shader.setFloat("time", time); // For flicker effect in the shader.
    
    shader.setFloat("fogNear", 4.0f);
    shader.setFloat("fogFar", 7.0f);
    shader.setVec3("fogColor", 0.02f, 0.02f, 0.04f);
}

/**
 * @brief Renders the main level geometry.
 */
void Renderer::renderLevel() {
    if (!levelShader || !level) return;
    
    setupLighting(*levelShader, static_cast<float>(glfwGetTime()));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
    
    glm::mat4 view = gameState->camera.GetViewMatrix();
    
    levelShader->setMat4("model", model);
    levelShader->setMat4("view", view);
    levelShader->setMat4("projection", gameState->projection);

    level->Draw(*levelShader);
}

/**
 * @brief Renders the bonfire, switching between the sword and lit states.
 * @param flag True if the bonfire is lit (player has the sword), false otherwise.
 */
void Renderer::renderBonfire(bool flag) {
    if (!bonfireShader || !bonfire || !bonfireSword) return;

    setupTorchLighting(*bonfireShader, static_cast<float>(glfwGetTime()));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    
    glm::mat4 view = gameState->camera.GetViewMatrix();

    bonfireShader->setMat4("model", model);
    bonfireShader->setMat4("view", view);
    bonfireShader->setMat4("projection", gameState->projection);

    // Render the unlit bonfire (with sword) or the lit bonfire.
    if (!flag){
        bonfireSword->Draw(*bonfireShader);
    } else {
        bonfire->Draw(*bonfireShader);
    }
}

/**
 * @brief Renders the player's first-person sword model.
 * @param type A string indicating which sword model to render (e.g., "broken").
 */
void Renderer::renderSword(string type) {
    if (!swordShader || !sword || !brokenSword) return;
    
    setupLighting(*swordShader, static_cast<float>(glfwGetTime()));
    
    // Clear the depth buffer to ensure the sword renders on top of the scene.
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 swordModel = glm::mat4(1.0f);
    
    // Position the sword relative to the camera for a first-person view.
    glm::vec3 swordPos = gameState->camera.Position + 
                        gameState->camera.Front * 0.6f +
                        gameState->camera.Right * 0.6f -
                        gameState->camera.Up * 0.25f;
    
    // Apply a subtle bobbing effect synchronized with player movement.
    float swordBobOffset = sin(gameState->bobTimer) * (BOB_AMOUNT * 0.2f);
    swordPos.y += swordBobOffset;
    
    swordModel = glm::translate(swordModel, swordPos);
    
    // Align the sword with the camera's rotation.
    float cameraYaw = atan2(gameState->camera.Front.x, gameState->camera.Front.z);
    float cameraPitch = asin(-gameState->camera.Front.y);
    swordModel = glm::rotate(swordModel, cameraYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    float limitedPitch = glm::clamp(cameraPitch * 0.8f, glm::radians(-70.0f), glm::radians(70.0f));
    swordModel = glm::rotate(swordModel, limitedPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Apply a fixed orientation to position the sword correctly in the view.
    swordModel = glm::rotate(swordModel, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    swordModel = glm::rotate(swordModel, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    swordModel = glm::rotate(swordModel, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    glm::mat4 view = gameState->camera.GetViewMatrix();
    
    swordShader->setMat4("model", swordModel);
    swordShader->setMat4("view", view);
    swordShader->setMat4("projection", gameState->projection);
    
    if (type == "broken"){
        brokenSword->Draw(*swordShader);
    } else {
        // Currently, only the broken sword is rendered.
        // sword->Draw(*swordShader);
    }
}

/**
 * @brief The main render loop function, called once per frame.
 */
void Renderer::render() {
    // Clear the screen with a dark blue color to match the PS1 aesthetic.
    glClearColor(0.01f, 0.01f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the projection matrix based on the current camera zoom and aspect ratio.
    gameState->projection = glm::perspective(
        glm::radians(gameState->camera.Zoom), 
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 
        0.1f, 100.0f
    );

    // Render all scene components in order.
    renderLevel();
    renderBonfire(gameState->hasBrokenSword);
    renderSword(gameState->swordType);
}