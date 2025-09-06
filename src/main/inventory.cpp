/**
 * @file inventory.cpp
 * @brief Implementation of the Item and Inventory classes for managing player items.
 */

#include <iostream>
#include "inventory.h"
#include <algorithm> // Required for std::erase_if

/**
 * @brief Constructs an Item with a name, quantity, and image path.
 * @param name The display name of the item.
 * @param quantity The number of this item.
 * @param imagePath The file path to the item's UI texture.
 */
Item::Item(const std::string& name, const std::string& description, int quantity, const std::string& imagePath)
    : name(name), description(description), quantity(quantity), imagePath(imagePath) {}

/**
 * @brief Gets the name of the item.
 * @return The item's name as a const string reference.
 */
std::string Item::getName() const {
    return name;
}

/**
 * @brief Gets the current quantity of the item.
 * @return The item's quantity.
 */
int Item::getQuantity() const {
    return quantity;
}

/**
 * @brief Gets the file path for the item's icon.
 * @return The item's image path as a const string reference.
 */
std::string Item::getImagePath() const {
    return imagePath;
}

std::string Item::getDescription() const {
    return description;
}

/**
 * @brief Sets the quantity of the item.
 * @param newQuantity The new quantity for the item.
 */
void Item::setQuantity(int newQuantity) {
    this->quantity = newQuantity;
}

/**
 * @brief Constructs an Inventory, optionally with a pre-defined list of items.
 * @param itemsList A vector of items to initialize the inventory with.
 */
Inventory::Inventory(std::vector<Item> itemsList)
    : items(std::move(itemsList)) {}
    
/**
 * @brief Adds a new item to the inventory.
 * @param item The item to add.
 */
void Inventory::addItem(const Item& item) {
    // In a more complex system, you might check if the item already exists
    // and just increase the quantity. For now, we simply add it.
    items.push_back(item);
}

/**
 * @brief Removes all items with a matching name from the inventory.
 * @param itemName The name of the item(s) to remove.
 */
void Inventory::removeItem(const std::string& itemName) {
    // std::erase_if is a C++20 feature that simplifies the erase-remove idiom.
    std::erase_if(items, [&itemName](const Item& item) { 
        return item.getName() == itemName; 
    });
}
