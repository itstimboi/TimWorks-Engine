# TimWorks Engine 
> Previously known as **TimEngine**

This is the source code for my new engine!

# What is TimWorks Engine?

  TimWorks Engine is a game engine designed to be as cross-platform as possible.
I hope to achieve this by my core ideas for this engine:
  1. Using my own Windowing, Audio, and Input API, called TimWorks API, since SDL and GLFW often don't support those types of systems.
  2. Supporting software-rendering, while most other engines don't, even engines that support retro games that used them. (e.g. Doom, Quake, Half-Life, Duke Nukem)
  3. Supporting out-of-date graphics APIs, while most other engines don't, even engines that support retro games that used them. (e.g. Quake, Half-Life, Halo: Combat Evolved)
  4. Aiming to make older rendering techniques and graphics APIs accessible alongside newer technologies feasible in one binary.

# Why are there other folders in the directory?

Besides the Engine, Build, include, and lib folders, the repository contains several folders used for game development, tools, experiments, and older code.
  
The "Level Editor" folder is for the game I'm planning to make with this engine, not the engine itself.

The "Model Maker" folder contains two subdirectories, ".tw_mdl File Importer", which contains a Blender addon for my model format, and "OLDSTUFF", which contains a failed attempt at a modeling software.
The "Builder" folder contains an application that makes compiling more fun for me.

The "OldEngineStuff" folder contains out-dated code files that I removed.

The "Game" folder contains code for the game I'm planning to make with this engine.

The "GameDLL" folder contains code for the game to use, not for the engine.

These folders are part of the broader development process, but not all of them are components of the engine itself.

# Project Status

TimWorks Engine is still in progress, but has a basis that still works for an engine. Features, APIs, rendering backends, and platform support may change as development continues.

  ### Supported Features

  1. Win32 Window Creation
  2. OpenGL 4.6

  ### In-Progress Features
  
  1. OpenGL 1.0 (Still in progress)
  2. TimWorks API inclusion

  ### Planned Features
  
  1. Other Window Backends
  2. Software Rendering
  3. All Other OpenGL versions
  4. All Vulkan Versions
  5. All DirectX versions
  6. Other Graphics APIs

# License

See the license file included in this repository for the terms governing the use, modification, and distribution of TimWorks Engine.
