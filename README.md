# libntr
Drop-in replacement for NitroSDK that targets Nintendo DS, PC, and other platforms.

libntr makes it easier to port Nintendo DS games to PC and other platforms by handling the logic for common libraries used by nearly all Nintendo DS games.

## Features
 * Fully compatible with NitroSDK
 * Translate DS 3D graphics APIs to OpenGL 4.2 with support for 3D upscaling
 * Simulate DS 2D graphics
 * Includes support for adding GUIs via Dear ImGui
 * Simulate local wireless multiplayer over LAN or the internet

## Available Build Targets
Select the build target with the 'build_target' meson option.
 * arm (Nintendo DS hardware)
 * linux (x86_64 Linux PC)
 * win64 (x86_64 Windows PC)
 * nx (Switch via libnx)
