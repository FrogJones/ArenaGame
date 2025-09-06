/**
 * @file interactionSystem.cpp
 * @brief Manages all interactable objects in the game world.
 */

#include "interactionSystem.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>

/**
 * @brief Initializes the interaction system. Currently does nothing but is available for future setup.
 * @return Always true.
 */
bool InteractionSystem::initialize() {
    // All interactions are now added externally (e.g., in GameEngine).
    return true;
}

/**
 * @brief Adds a new interactable object to the system.
 * @param pos The world-space position of the object.
 * @param text The prompt text to display to the player (e.g., "E - Open").
 * @param callback The function to execute when the player interacts with the object.
 */
void InteractionSystem::AddInteractable(const glm::vec3& pos, const std::string& text, std::function<void()> callback) {
    // Emplace_back is slightly more efficient than push_back as it constructs the object in-place.
    interactables.emplace_back(pos, 1.0f, text, std::move(callback));
}

/**
 * @brief Checks if the player is within range of any interactable objects.
 * @param playerPos The current position of the player's camera.
 * @param outText [out] The prompt text of the nearest interactable object.
 * @return True if the player is near an object, false otherwise.
 */
bool InteractionSystem::CheckInteractions(const glm::vec3& playerPos, std::string& outText) {
    for (const auto& obj : interactables) {
        // Skip objects that have already been used up.
        if (obj.consumed) continue;

        float distance = glm::length(playerPos - obj.position);
        if (distance <= obj.radius) {
            outText = obj.promptText;
            return true; // Found an object in range.
        }
    }
    return false; // No objects in range.
}

/**
 * @brief Executes the interaction for the nearest object to the player.
 * @param playerPos The current position of the player's camera.
 * @return True if an interaction was successfully handled, false otherwise.
 */
bool InteractionSystem::HandleInteraction(const glm::vec3& playerPos) {
    for (auto& obj : interactables) {
        if (obj.consumed) continue;

        float distance = glm::length(playerPos - obj.position);
        if (distance <= obj.radius) {
            // Execute the associated callback function.
            if (obj.onInteract) {
                obj.onInteract();
            }
            // Mark the object as consumed to prevent re-interaction.
            obj.consumed = true;
            return true;
        }
    }
    return false;
}

/**
 * @brief Removes all interactable objects from the system.
 */
void InteractionSystem::ClearInteractables() {
    interactables.clear();
}

/**
 * @brief Removes a specific interactable object based on its position.
 * @param pos The position of the object to remove.
 * @param tol A tolerance radius to account for floating-point inaccuracies.
 * @return True if an object was found and removed, false otherwise.
 */
bool InteractionSystem::RemoveInteractable(const glm::vec3& pos, float tol) {
    // Use the erase-remove idiom to efficiently remove all matching elements.
    auto it = std::remove_if(interactables.begin(), interactables.end(),
        [&](const InteractableObject& obj) {
            return glm::length(obj.position - pos) <= tol;
        });

    if (it != interactables.end()) {
        interactables.erase(it, interactables.end());
        return true;
    }
    return false;
}