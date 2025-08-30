# 🕹️ PS1-Style Level Viewer

A retro PlayStation 1-inspired 3D level viewer built with C++, OpenGL, and OpenAL. Experience authentic low-poly environments with pixelated textures, atmospheric torch lighting, and immersive footstep audio that captures the nostalgic feel of classic 90s gaming.

![PS1 Level Viewer Screenshot](docs/images/screenshot-main.png)
*Main level view showcasing the retro PS1 aesthetic*

## ✨ Features

### 🎨 Authentic PS1 Visuals
- **Pixelated Textures**: Nearest-neighbor filtering for that classic blocky look
- **Low-Poly Models**: Minimalist geometry true to PS1 limitations
- **Distance Fog**: Atmospheric fog effect for authentic depth limitation
- **Retro Color Palette**: Carefully chosen colors that evoke 90s gaming nostalgia

### 💡 Dynamic Lighting System
- **Multiple Point Lights**: Realistic torch illumination throughout the level
- **Flickering Effects**: Dynamic torch flames with subtle intensity variations
- **Minimal Fill Light**: Subtle directional lighting to maintain visibility

### 🎵 Immersive Audio Experience
- **Positional Footsteps**: Audio triggers only during movement with realistic timing
- **Randomized Sound Effects**: Multiple footstep samples prevent repetitive audio
- **Spatial Audio**: 3D positioned audio using OpenAL for realistic sound placement
- **Ambient Soundscape**: Atmospheric audio layers for enhanced immersion

### 🎮 Intuitive Controls
- **Free-Look Camera**: Smooth mouse-controlled camera with configurable sensitivity
- **WASD Movement**: Standard FPS-style movement with momentum
- **Camera Bobble**: Subtle walking animation for authentic feel
- **Boundary Detection**: Smart collision system keeps camera within level bounds

## 🎮 Controls Reference

| Input | Action | Description |
|-------|--------|-------------|
| **Mouse Move** | Look Around | Free-look camera control |
| **W** | Move Forward | Walk forward in view direction |
| **A** | Strafe Left | Move left relative to camera |
| **S** | Move Backward | Walk backward from view direction |
| **D** | Strafe Right | Move right relative to camera |
| **ESC** | Toggle Menu | Unlock cursor and open settings |
| **Left Click** | Interact | Relock cursor after menu use |

## 🔬 Technical Implementation

### Rendering Pipeline

![Rendering Pipeline](docs/images/rendering-pipeline.png)
*OpenGL rendering pipeline flow*

The rendering system implements several key PS1-era techniques:

- **Limited Color Depth**: Reduces color precision to match original hardware limitations
- **Fog Implementation**: Distance-based fog using OpenGL's built-in fog functions
- **More Planned**

### Audio System Architecture

The audio implementation focuses on performance and authenticity:

- **Streaming Audio**: Efficient WAV file streaming via libsndfile
- **3D Positional Audio**: OpenAL-based spatial audio for realistic sound placement
- **Dynamic Loading**: On-demand audio resource management
- **Low Latency Playback**: Optimized for responsive footstep audio triggering

### Camera and Movement System

![Camera System](docs/images/camera-system.png)
*Camera movement and collision detection*

## 🛣️ Development Roadmap

### 🎯 Short Term Goals
- [ ] **Vertex Precision Reduction**: Simulates PS1's limited vertex precision for authentic jitter
- [ ] **Affine Texture Mapping**: Recreates the characteristic texture warping of PS1 graphics
- [ ] **Enhanced Menu System**: Pause menu with graphics and audio settings
- [ ] **Multiple Level Support**: Level selection and seamless transitions
- [ ] **Improved Camera Bobble**: More realistic walking animation synchronization
- [ ] **Performance Optimization**: Frame rate improvements and memory usage reduction
- [ ] **Configuration System**: User preferences saving and loading

### 🚀 Long Term Vision
- [ ] **Particle Effects**: PS1-style sprite-based particles for dust and fire effects
- [ ] **Expanded Audio Library**: Additional ambient sounds and music
- [ ] **Advanced Shading**: Custom PS1-style vertex lighting and texture effects
- [ ] **Interactive Objects**: Basic interaction system for switches and doors

## 🔧 Troubleshooting

### Common Issues

**Issue**: -
**Solution**: -

**Issue**: -
**Solution**: -

**Issue**: -
**Solution**: -

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License © 2024 Panagiotis Fragkakis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files...
```
<div align="center">
 
**Made with ❤️ for retro gaming enthusiasts**

[⭐ Star this project](https://github.com/FrogJones/ArenaGame) • [🐛 Report Bug](https://github.com/yourusername/ps1-level-viewer/issues)

</div>
