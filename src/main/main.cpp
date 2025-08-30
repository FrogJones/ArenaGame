#include <iostream>
#include "gameEngine.h"

int main() {
    GameEngine engine;
    
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize game engine" << std::endl;
        return -1;
    }
    
    engine.run();
    
    std::cout << "PS1-style level viewer completed successfully!" << std::endl;
    return 0;
}