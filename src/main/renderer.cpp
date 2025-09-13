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
      swordShader(nullptr),
      bonfireShader(nullptr),
      level(nullptr), 
      bonfire(nullptr),
      bonfireSword(nullptr),
      brokenSword(nullptr),
      sword(nullptr),
      lightBeam(nullptr) 
{
}

Renderer::~Renderer() {
    delete levelShader;
    delete swordShader;
    delete bonfireShader;
    delete level;
    delete bonfireSword;
    delete bonfire;
    delete brokenSword;
    delete sword;
    delete lightBeam;
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
        lightBeam = new Model("models/lightBeam/lightBeam.obj");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load models: " << e.what() << std::endl;
        return false;
    }
}

void Renderer::setupLighting(Shader& shader, float time) {
    shader.use();

    shader.setVec3("viewPos", gameState->camera.Position);

    shader.setVec3("dirLight.direction", DIR_LIGHT_DIRECTION);
    shader.setVec3("dirLight.ambient", DIR_LIGHT_AMBIENT);
    shader.setVec3("dirLight.diffuse", DIR_LIGHT_DIFFUSE);
    shader.setVec3("dirLight.specular", DIR_LIGHT_SPECULAR);

    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        std::string number = std::to_string(i);
        shader.setVec3("pointLights[" + number + "].position", POINT_LIGHT_POSITIONS[i]);
        
        if (i == BONFIRE_LIGHT_INDEX) {
            float flicker = FLICKER_BASE + FLICKER_AMPLITUDE * sin(time * FLICKER_FREQ1 + i * FLICKER_PHASE1) * sin(time * FLICKER_FREQ2 + i * FLICKER_PHASE2);
            
            shader.setVec3("pointLights[" + number + "].ambient", BONFIRE_AMBIENT_BASE * flicker);
            shader.setVec3("pointLights[" + number + "].diffuse", BONFIRE_DIFFUSE_BASE * flicker);
            shader.setVec3("pointLights[" + number + "].specular", BONFIRE_SPECULAR);
            
            shader.setFloat("pointLights[" + number + "].constant", LIGHT_CONSTANT);
            shader.setFloat("pointLights[" + number + "].linear", BONFIRE_LINEAR);
            shader.setFloat("pointLights[" + number + "].quadratic", BONFIRE_QUADRATIC);
        } else {
            shader.setVec3("pointLights[" + number + "].ambient", REGULAR_LIGHT_COLOR);
            shader.setVec3("pointLights[" + number + "].diffuse", REGULAR_LIGHT_COLOR);
            shader.setVec3("pointLights[" + number + "].specular", REGULAR_LIGHT_COLOR);
            
            shader.setFloat("pointLights[" + number + "].constant", LIGHT_CONSTANT);
            shader.setFloat("pointLights[" + number + "].linear", REGULAR_LINEAR);
            shader.setFloat("pointLights[" + number + "].quadratic", REGULAR_QUADRATIC);
        }
    }

    shader.setFloat("material.shininess", MATERIAL_SHININESS);
    shader.setFloat("material.alpha", MATERIAL_ALPHA);

    shader.setFloat("fogNear", FOG_NEAR);
    shader.setFloat("fogFar", FOG_FAR);
    shader.setVec3("fogColor", FOG_COLOR);
}

void Renderer::setupTorchLighting(Shader& shader, float time) {
    shader.use();
    
    shader.setVec3("viewPos", gameState->camera.Position);
    
    shader.setVec3("dirLight.direction", DIR_LIGHT_DIRECTION);
    shader.setVec3("dirLight.ambient", TORCH_DIR_AMBIENT);
    shader.setVec3("dirLight.diffuse", TORCH_DIR_DIFFUSE);
    shader.setVec3("dirLight.specular", DIR_LIGHT_SPECULAR);
    
    shader.setFloat("material.shininess", TORCH_SHININESS);
    shader.setFloat("material.emissiveStrength", TORCH_EMISSIVE_STRENGTH);
    shader.setFloat("time", time);
    
    // Atmospheric fog settings
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
void Renderer::renderSword(std::string type) {
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
 * @brief Renders the atmospheric light beam from the ceiling using volumetric layers.
 */
void Renderer::renderLightBeam() {
    if (!levelShader || !lightBeam) return;
    
    // Configure alpha blending for light beam transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    
    // Configure bright self-illuminated lighting for the light beam
    levelShader->use();
    levelShader->setVec3("viewPos", gameState->camera.Position);
    
    levelShader->setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    levelShader->setVec3("dirLight.ambient", 2.5f, 2.5f, 2.0f);
    levelShader->setVec3("dirLight.diffuse", 2.5f, 2.5f, 2.0f);
    levelShader->setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
    
    // Disable all point lights for the light beam
    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        std::string number = std::to_string(i);
        levelShader->setVec3("pointLights[" + number + "].diffuse", 0.0f, 0.0f, 0.0f);
        levelShader->setVec3("pointLights[" + number + "].ambient", 0.0f, 0.0f, 0.0f);
        levelShader->setVec3("pointLights[" + number + "].specular", 0.0f, 0.0f, 0.0f);
    }
    
    levelShader->setFloat("material.shininess", 1.0f);
    
    // Disable fog for the light beam
    levelShader->setFloat("fogNear", 999.0f);
    levelShader->setFloat("fogFar", 1000.0f);

    glm::mat4 view = gameState->camera.GetViewMatrix();
    levelShader->setMat4("view", view);
    levelShader->setMat4("projection", gameState->projection);

    // Render volumetric light beam using multiple layered cones
    const glm::vec3 beamPosition(0.0f, 2.5f, 0.0f);
    const glm::vec3 beamScale(1.0f, 2.5f, 1.0f);
    
    // Layer 1: Outer cone
    levelShader->setFloat("material.alpha", 0.15f);
    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, beamPosition);
    model1 = glm::scale(model1, beamScale * glm::vec3(1.4f, 1.0f, 1.4f));
    levelShader->setMat4("model", model1);
    lightBeam->Draw(*levelShader);
    
    // Layer 2: Middle-outer cone
    levelShader->setFloat("material.alpha", 0.25f);
    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, beamPosition);
    model2 = glm::scale(model2, beamScale * glm::vec3(1.1f, 1.0f, 1.1f));
    levelShader->setMat4("model", model2);
    lightBeam->Draw(*levelShader);
    
    // Layer 3: Middle-inner cone
    levelShader->setFloat("material.alpha", 0.35f);
    glm::mat4 model3 = glm::mat4(1.0f);
    model3 = glm::translate(model3, beamPosition);
    model3 = glm::scale(model3, beamScale * glm::vec3(0.8f, 1.0f, 0.8f));
    levelShader->setMat4("model", model3);
    lightBeam->Draw(*levelShader);
    
    // Layer 4: Inner core
    levelShader->setFloat("material.alpha", 0.5f);
    glm::mat4 model4 = glm::mat4(1.0f);
    model4 = glm::translate(model4, beamPosition);
    model4 = glm::scale(model4, beamScale * glm::vec3(0.5f, 1.0f, 0.5f));
    levelShader->setMat4("model", model4);
    lightBeam->Draw(*levelShader);
    
    // Restore normal rendering state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

/**
 * @brief The main render loop function, called once per frame.
 */
void Renderer::render() {
    // Clear the screen with a dark blue color to match the PS1 aesthetic.
    glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the projection matrix based on the current camera zoom and aspect ratio.
    gameState->projection = glm::perspective(
        glm::radians(gameState->camera.Zoom), 
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 
        0.1f, 100.0f
    );

    // Render all scene components in order
    renderLevel();
    renderBonfire(gameState->hasBrokenSword);
    renderSword(gameState->swordType);
    renderLightBeam();
}