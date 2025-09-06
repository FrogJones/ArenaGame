#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

struct InteractableObject {
    glm::vec3 position;
    float radius = 1.0f;
    std::string promptText;
    std::function<void()> onInteract;
    bool consumed = false; // To mark for removal
};

class InteractionSystem {
public:
    InteractionSystem() = default;
    ~InteractionSystem() = default;

    bool initialize();
    void AddInteractable(const glm::vec3& pos, const std::string& text, std::function<void()> callback);
    bool CheckInteractions(const glm::vec3& playerPos, std::string& outText);

    // Changed: return true if an interaction occurred (and object consumed/removed)
    bool HandleInteraction(const glm::vec3& playerPos);

    // Optional helpers
    bool RemoveInteractable(const glm::vec3& pos, float tol = 0.1f);
    void ClearInteractables();

private:
    std::vector<InteractableObject> interactables;
};