#ifndef ITEMS_H
#define ITEMS_H

#include "inventory.h"

namespace Items {
    // Create predefined items with all their properties
    inline Item BrokenSword() {
        return Item(
            "Broken Sword",
            R"(Found wedged within the ashes of a long-dead bonfire.

Too frail to serve as a true weapon, it will not carry one far. Yet a faint warmth clings to the steel, as though the flame refuses to release it.

left by a nameless warrior who sought comfort in the fire, entrusting their last strength to the embers.)",
            1,
            "icons/broken_sword.png"
        );
    }
    
}

#endif
