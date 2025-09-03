#include "interactionSystem.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

bool InteractionSystem::initialize() {
    // Add default interactable for testing; callers can add more later.
    AddInteractable(glm::vec3(0.0f, 1.0f, 0.0f), "E - Pull Out", [](){
        std::cout << "Broken sword acquired...\n";
    });
    return true;
}

void InteractionSystem::AddInteractable(const glm::vec3& pos, const std::string& text, std::function<void()> callback) {
    InteractableObject obj;
    obj.position = pos;
    obj.promptText = text;
    obj.onInteract = std::move(callback);
    interactables.push_back(std::move(obj));
}

bool InteractionSystem::CheckInteractions(const glm::vec3& playerPos, std::string& outText) {
    for (const auto& obj : interactables) {
        float distance = glm::length(playerPos - obj.position);
        if (distance <= obj.radius) {
            outText = obj.promptText;
            return true;
        }
    }
    return false;
}

void InteractionSystem::HandleInteraction(const glm::vec3& playerPos) {
    for (auto& obj : interactables) {
        float distance = glm::length(playerPos - obj.position);
        if (distance <= obj.radius) {
            if (obj.onInteract) obj.onInteract();
            break;
        }
    }
}