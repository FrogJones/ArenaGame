#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

struct InteractableObject {
    glm::vec3 position;
    float radius = 1.0f;
    std::string promptText;
    std::string popupText;
    std::function<void()> onInteract;
    bool consumed = false;

    // Constructor to ensure correct member initialization
    InteractableObject(glm::vec3 pos, float r, std::string prompt, std::string popup, std::function<void()> cb)
        : position(pos), radius(r), promptText(std::move(prompt)), popupText(std::move(popup)), 
          onInteract(std::move(cb)), consumed(false) {}
};

class InteractionSystem {
public:
    InteractionSystem() = default;
    ~InteractionSystem() = default;

    bool initialize();
    void AddInteractable(const glm::vec3& pos, const std::string& text, const std::string& popup, std::function<void()> callback);
    bool CheckInteractions(const glm::vec3& playerPos, std::string& outText);

    // Changed: return true if an interaction occurred (and object consumed/removed)
    bool HandleInteraction(const glm::vec3& playerPos, std::string& outPopup);

    // Optional helpers
    void ClearInteractables();

private:
    std::vector<InteractableObject> interactables;
};