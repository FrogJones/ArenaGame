#include <iostream>
#include "gameEngine.h"

int main() {
    GameEngine engine;
    
    if (!engine.initialize()) {
        std::cerr << "FATAL: Game engine failed to initialize. Exiting." << std::endl;
        return -1;
    }
    
    engine.run();
    
    std::cout << "Application exited successfully." << std::endl;
    return 0;
}