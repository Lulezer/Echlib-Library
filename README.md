# Echlib

Echlib is an open-source C++ game development library that I'm building to make creating games in C++ easier, especially for beginners.

I originally started Echlib as a learning project, but over time it turned into something much bigger.

**A little note about the rewrite:** I decided to rewrite Echlib from scratch because the old code was getting messy, unoptimized, and hard to work with. The new version has a much cleaner structure, better performance, and is easier to maintain and expand.

## Features

### Currently implemented

- [x] Rendering (textures and shapes)
- [x] Audio
- [x] Keyboard and mouse input
- [x] 2D camera
- [x] Basic collision detection
- [x] File I/O
- [x] Text and UI

### Planned

- [ ] Scene management
- [ ] Particle system
- [ ] Cross-platform support

### Things I might add later

- [ ] Scripting and event handling
- [ ] Networking
- [ ] AI and pathfinding

Nothing here is set in stone. I'll probably add, change, or remove things as development continues.

## What's next?

For now, my goal is to make Echlib a solid and easy-to-use game development library.

Eventually, I'd love to turn it into a full game engine, but there's still a lot of work to do before that happens.

## Getting started

### Manual installation

1. Download the latest version from the [Releases](https://github.com/Lulezer/Echlib-Library/releases) page.
2. Extract the ZIP file.
3. Copy the `include/` and `src/` folders into your project.
4. Add the necessary source files and configure the include paths in your IDE or build system.
5. Include `echlib.hpp` and you're ready to start.

## Credits

A huge thanks to:

- [MrLLGD (Low Level Game Dev)](https://www.youtube.com/@lowlevelgamedev9330/videos): For helping me with Echlib in its early days, teaching me a lot about OpenGL and CMake, going through my code with me, and sharing examples from his own library. Echlib probably wouldn't be where it is today without his help.

- [Raysan5](https://github.com/raysan5): For creating `raudio`, which Echlib uses for audio.

## Documentation

I'm still working on the documentation. Once it's ready, you'll be able to find it on the [Echlib Wiki](https://github.com/Lulezer/Echlib-Library/wiki).

Thanks for checking out Echlib!
