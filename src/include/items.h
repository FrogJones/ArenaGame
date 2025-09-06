#ifndef ITEMS_H
#define ITEMS_H

#include "inventory.h"

namespace Items {
    // Create predefined items with all their properties
    inline Item BrokenSword() {
        return Item(
            "Broken Sword",
            "A sword that has seen better days. The blade is chipped and dull.",
            1,
            "icons/broken_sword.png"
        );
    }
    
}

#endif
