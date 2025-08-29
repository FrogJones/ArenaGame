
🕹️ PS1-Style Level Viewer

A retro PlayStation 1-inspired 3D level viewer built with C++, OpenGL, and OpenAL, featuring low-poly environments, pixelated textures, torch lighting, and footstep audio.

✨ Features

Retro PS1 Visuals: Pixelated textures with nearest-neighbor filtering, low-poly level and object models, fog for distance limitation.

Dynamic Lighting: Multiple point lights for torches, minimal directional fill light, flickering torch effect.

Camera: FPS style with mouse and WASD movement.

Audio: Footstep sounds.

🎮 Controls
Input	Action
Mouse Move	Look around
W / A / S / D	Move forward / left / backward / right
ESC	Unlock cursor / open menu
Left Click	Relock cursor / attack
🚀 Getting Started
Prerequisites

C++ Compiler (GCC 7+, Clang 5+, or MSVC 2017+)

CMake 3.10 or higher

GLFW 3.3+

GLAD (OpenGL 3.3 loader)

GLM for mathematics

OpenAL Soft for audio

libsndfile for WAV loading

stb_image for textures

All dependencies can be installed via vcpkg or your preferred package manager.

Building

Clone the repository:
git clone https://github.com/yourusername/ps1-level-viewer.git && cd ps1-level-viewer

Create build directory:
mkdir build && cd build

Generate build files:
cmake ..

Compile:
cmake --build . --config Release

Run:
./PS1LevelViewer

🏗️ Project Structure
ps1-level-viewer/
├── src/
│   ├── main/
│       ├── main.cpp      # Main application logic
│   ├── include/          # All custom helper classes
│   ├── shaders/          # GLSL shaders for level and torches
│   ├── models/           # Low-poly 3D models and materials
│   └── sfx/              # Footstep and ambient sounds
├── build/                # Build artifacts (ignored by git)
├── .gitignore
└── README.md

🔬 Technical Details
Rendering

Pixelated Textures using nearest filtering

Low-poly Models with minimal polygons

PS1 Fog Effect for distance limitation

Torch Lighting with positions and rotations set to walls and corners

Audio

Footstep Sounds triggered only when moving beyond a small threshold

Randomized Step Sounds to prevent repetition

OpenAL Soft handles audio playback

WAV Files via libsndfile for simplicity and low latency

Camera & Movement

WASD Movement with optional vertical movement

Mouse Look with pitch and yaw rotation

Walking Bobble for retro feel

Movement Boundaries to keep camera inside level

🛣️ Roadmap
Short Term

 Implement menus for pause, settings, and audio control

 Add multiple levels

 Enhance camera bobble and footstep syncing

Long Term

 Particle effects for dust and fire

 Expand audio library with ambient sounds

 Shader enhancements for PS1-style lighting tricks


📝 License

MIT License © Panagiotis Fragkakis

