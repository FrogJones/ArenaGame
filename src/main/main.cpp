#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
// Graphics and windowing libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// GLM for mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// OpenAL for audio
#include <AL/al.h>
#include <AL/alc.h>
// audio file loading
#include <sndfile.h>
// Custom classes
#include <shader.h>
#include <camera.h>
#include <model.h>
#include <audioManager.h>
// Image loading
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>

// ====================== CONSTANTS & CONFIGURATION ======================
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
glm::mat4 projection; // for aspect ratio handling

bool cursorLocked = true;
bool awaitingRelock = false;


// ====================== GLOBAL STATE ======================
// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse handling
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Light positions for arena corner lighting
glm::vec3 pointLightPositions[] = {
    glm::vec3(-2.7f, 1.2f, -2.9f),  // Back-left corner
    glm::vec3( 2.7f, 1.2f, -2.9),  // Back-right corner
    glm::vec3( 2.7f, 1.2f,  2.9f),  // Front-right corner
    glm::vec3(-2.7f, 1.2f,  2.9f),   // Front-left corner
    glm::vec3( 0.0f, 1.2f, -2.9f),   // Mid-back
    glm::vec3( 2.9f, 1.2f,  0.0f),    // Mid-right
    glm::vec3( 0.0f, 1.2f,  2.9f),    // Mid-front
    glm::vec3(-2.9f, 1.2f,  0.0f)   // Mid-left
};

// ====================== CALLBACK FUNCTIONS ======================
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    float aspect = (float)width / (float)height;
    projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.01f, 20.0f);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    
    if (!cursorLocked) return; // ignore mouse movement when cursor is unlocked
    
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (awaitingRelock && !cursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // relock
            cursorLocked = true;
            awaitingRelock = false;
            firstMouse = true; // reset mouse delta to avoid jump
        }
        else if (cursorLocked) {
            // Normal left click action (shoot/attack/etc.)
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window) {
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && cursorLocked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // unlock
        cursorLocked = false;
        awaitingRelock = true; // wait for first click
    }

    // Camera movement using WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    //     camera.ProcessKeyboard(UP, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    //     camera.ProcessKeyboard(DOWN, deltaTime);
}
// ====================== INITIALIZATION FUNCTIONS ======================
GLFWwindow* initializeGLFW() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PS1 Level Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

bool initializeGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }
    return true;
}

void updateTiming() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
// ====================== LIGHTING SETUP FUNCTION ======================
void setupLighting(Shader& shader, float time) {
    shader.use();

    // Set view position
    shader.setVec3("viewPos", camera.Position);

    // Directional light (minimal - just basic fill light)
    shader.setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    shader.setVec3("dirLight.ambient", 0.005f, 0.005f, 0.01f);
    shader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.08f);
    shader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);

    // Point lights with flicker
    for (int i = 0; i < 8; i++) {
        std::string number = std::to_string(i);
        glm::vec3 lightPos = pointLightPositions[i];
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
// ====================== TORCH LIGHTING SETUP FUNCTION ======================
void setupTorchLighting(Shader& shader, float time) {
    shader.use();
    
    // Set view position
    shader.setVec3("viewPos", camera.Position);
    
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
//===================== AUDIO SETUP FUNCTION ======================
void setupAudio(ALCdevice* &device, ALCcontext* &context) {
    device = alcOpenDevice(nullptr); // Open default device
    if (!device) {
        std::cerr << "Failed to open OpenAL device\n";
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cerr << "Failed to create OpenAL context\n";
        if (context) alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }

    std::cout << "OpenAL device initialized successfully!\n";
}
// ====================== MAIN FUNCTION ======================
int main() {

    ALCdevice* device = nullptr;

    if (!alcIsExtensionPresent(device, "ALC_EXT_EFX")) {
    std::cerr << "EFX not supported!\n";    
    }

    // Initialize GLFW and create window
    GLFWwindow* window = initializeGLFW();
    if (!window) return -1;

    // Initialize GLAD
    if (!initializeGLAD()) return -1;

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);
    
    // Set texture filtering to nearest for PS1 pixelated look
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Flip textures vertically on load (for models)
    stbi_set_flip_vertically_on_load(true);

    // Build and compile shader program
    Shader levelShader("shaders/levelVs.glsl", "shaders/levelFs.glsl");
    Shader torchShader("shaders/torchVs.glsl", "shaders/torchFs.glsl");
    Shader swordShader("shaders/swordVs.glsl", "shaders/swordFs.glsl");
    
    // Load level model
    Model level("models/level/level.obj");
    Model torch("models/torch/torch.obj");
    Model sword("models/sword/sword.obj");
    
    // audio setup
    AudioManager audio;
    if (!audio.init()) {
        std::cerr << "Failed to initialize AudioManager\n";
        return -1;
    }
    
    vector<ALuint> steps;
    for (int i = 1; i < 4; i++) {
        std::string filename = "sfx/steps/step" + std::to_string(i) + ".wav";
        ALuint buffer = audio.loadAudio(filename);
        if (buffer != 0) {
            steps.push_back(buffer);
        }
    }

    ALuint ambianceBuffer = audio.loadAudio("sfx/env/ambiance.wav");
    if (ambianceBuffer != 0) {
        ALuint ambianceSrc = audio.playSound(ambianceBuffer, true); // 'true' enables looping
        alSourcef(ambianceSrc, AL_GAIN, 0.2f); // Lower volume for ambiance
    }



    glm::vec3 lastCameraPos = camera.Position;
    float stepCooldown = 0.0f;
    float bobTimer = 0.0f;
    float bobAmount = 0.05f;
    float bobSpeed = 10.0f;
    
    
    // ====================== RENDER LOOP ======================
    while (!glfwWindowShouldClose(window)) {
        
        // Update timing first
        updateTiming();
        
        // Process input
        processInput(window);
        
        // Calculate movement distance
        float moveDistance = glm::length(camera.Position - lastCameraPos);
        
        // IMPORTANT: Store the base camera position before applying bob
        glm::vec3 baseCameraPos = camera.Position;
        baseCameraPos.y = 1.0f; // Fixed height
        
        // Apply movement boundaries BEFORE bobbing
        if (baseCameraPos.x > 2.8f) baseCameraPos.x = 2.8f;
        if (baseCameraPos.x < -2.8f) baseCameraPos.x = -2.8f;
        if (baseCameraPos.z > 2.8f) baseCameraPos.z = 2.8f;
        if (baseCameraPos.z < -2.8f) baseCameraPos.z = -2.8f;
        
        // Update camera position with boundaries applied
        camera.Position = baseCameraPos;
        
        // Threshold to avoid playing sound for very tiny movements
        if (moveDistance > 0.01f && stepCooldown <= 0.0f) {
            // Pick a random step sound
            int idx = rand() % steps.size();
            
            // Apply echo effect to footsteps for that dungeon feel
            ALuint stepSource = audio.playSoundWithEffect(steps[idx], "echo");
            
            // Optional: Vary volume and pitch slightly for more realistic steps
            float volumeVariation = 0.8f + (rand() % 40) / 100.0f; // 0.8 to 1.2
            float pitchVariation = 0.9f + (rand() % 20) / 100.0f;  // 0.9 to 1.1
            
            alSourcef(stepSource, AL_GAIN, volumeVariation * 0.6f);
            alSourcef(stepSource, AL_PITCH, pitchVariation);
            
            stepCooldown = 0.6f;
        }
        

        // Calculate bob only if moving
        float bobOffset = 0.0f;
        if (moveDistance > 0.01f) {
            bobTimer += deltaTime * bobSpeed;
            bobOffset = sin(bobTimer) * bobAmount;
        } else {
            // Gradually reset bob timer when standing still
            bobTimer = 0.0f;
        }
        
        // Apply bob to camera position
        camera.Position.y = 1.0f + bobOffset;

        // Update cooldown timer
        stepCooldown -= deltaTime;

        // Update last position for next frame (use the base position without bob)
        lastCameraPos = baseCameraPos;
        
        // Clear buffers with a very dark PS1-style background
        glClearColor(0.01f, 0.01f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate view matrix AFTER all camera position updates
        glm::mat4 view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), 
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                                0.1f, 100.0f);

        // Set up lighting (with time for animations)
        setupLighting(levelShader, static_cast<float>(glfwGetTime()));

        // Set up matrices for level
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        
        levelShader.setMat4("model", model);
        levelShader.setMat4("view", view);
        levelShader.setMat4("projection", projection);

        // Render the level
        level.Draw(levelShader);

        // Render torches
        setupTorchLighting(torchShader, static_cast<float>(glfwGetTime()));

        for (int i = 0; i < 8; i++) {
            glm::mat4 torchModel = glm::mat4(1.0f);
            torchModel = glm::translate(torchModel, pointLightPositions[i]);
            torchModel = glm::scale(torchModel, glm::vec3(1.0f, 1.0f, 1.0f));
            if (pointLightPositions[i].z < -1){
                torchModel = glm::rotate(torchModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            } else if (pointLightPositions[i].z > 1){
                torchModel = glm::rotate(torchModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            } else if (pointLightPositions[i].x < -1){
                torchModel = glm::rotate(torchModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            
            torchShader.setMat4("model", torchModel);
            torchShader.setMat4("view", view);
            torchShader.setMat4("projection", projection);
            
            torch.Draw(torchShader);
        }

        // RENDER SWORD: Setup lighting for sword shader
        setupLighting(swordShader, static_cast<float>(glfwGetTime()));
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 swordModel = glm::mat4(1.0f);
        // Position sword relative to camera with fixed offsets (like it's attached to the viewport)
        glm::vec3 swordPos = camera.Position + 
                            camera.Front * 0.6f +     // Fixed distance forward
                            camera.Right * 0.6f -    // Fixed distance to the right  
                            camera.Up * 0.25f;        // Fixed distance down
        
        // Apply only subtle bobbing effect
        float swordBobOffset = sin(bobTimer) * (bobAmount * 0.2f); // 20% of camera bob
        swordPos.y += swordBobOffset;
        
        swordModel = glm::translate(swordModel, swordPos);
        
        // Follow camera yaw rotation but keep pitch and roll static for stability
        float cameraYaw = atan2(camera.Front.x, camera.Front.z);
        float cameraPitch = asin(-camera.Front.y);
        swordModel = glm::rotate(swordModel, cameraYaw, glm::vec3(0.0f, 1.0f, 0.0f));
        float limitedPitch = glm::clamp(cameraPitch * 0.8f, glm::radians(-70.0f), glm::radians(70.0f));
        swordModel = glm::rotate(swordModel, limitedPitch, glm::vec3(1.0f, 0.0f, 0.0f));
        
        // Apply fixed sword orientation relative to camera direction
        swordModel = glm::rotate(swordModel, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Slight downward tilt
        swordModel = glm::rotate(swordModel, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Angle to the right
        swordModel = glm::rotate(swordModel, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // Slight roll
        
        swordShader.setMat4("model", swordModel);
        swordShader.setMat4("view", view);
        swordShader.setMat4("projection", projection);
        
        sword.Draw(swordShader);
    

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    std::cout << "PS1-style level viewer completed successfully!\n";
    return 0;
}