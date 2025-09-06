#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>

class Item {
public:
    Item(const std::string& name, int quantity, const std::string& imagePath);
    std::string getName() const;
    int getQuantity() const;
    void setQuantity(int quantity);
    std::string getImagePath() const;

private:
    std::string name;
    int quantity;
    std::string imagePath;
};

class Inventory {
public:
    Inventory(std::vector<Item> Items = {});
    void addItem(const Item& item);
    void removeItem(const std::string& itemName);
    Item* getItem(const std::string& itemName);
    std::vector<Item> getItems() const;

private:
    std::vector<Item> items;
};

#endif