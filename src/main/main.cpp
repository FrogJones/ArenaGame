/**
 * @file main.cpp
 * @brief The entry point of the application.
 *
 * This file contains the main function, which creates the GameEngine,
 * initializes it, runs the main loop, and handles graceful shutdown.
 */

#include <iostream>
#include "gameEngine.h"

/**
 * @brief Application entry point.
 * @return 0 on successful execution, -1 on initialization failure.
 */
int main() {
    // Create the game engine instance on the stack.
    GameEngine engine;
    
    // Initialize all subsystems. If any part fails, exit the application.
    if (!engine.initialize()) {
        std::cerr << "FATAL: Game engine failed to initialize. Exiting." << std::endl;
        return -1;
    }
    
    // Start the main game loop. This call will block until the user closes the window.
    engine.run();
    
    // The engine's destructor will handle all cleanup.
    std::cout << "Application exited successfully." << std::endl;
    return 0;
}