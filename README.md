# ActuallyAGameEngine

![image](https://user-images.githubusercontent.com/3623261/184268773-de19e916-4889-450d-9553-8c058959e6c0.png)

# Why AAGE?

AAGE is designed to be modular, and easy to modify. AAGE provides easy to use abstractions for OpenGL, Networking, Scripting, and more. The main use case of this engine is for creating 3D games, although other types of applications should be possible.

# Where AAGE?

AAGE only works on windows x64 at the moment, there is a working linux branch with only minor changes, it just needs to be updated and is low priority at the moment. 32 bit will not be supported.

# Dependancies 

- C++ 20 with MSVC / G++ 
- CMake
- Conan Package Manager
- OpenGL 4.6
- Ninja (optional)


# Template

You can find a starter template at https://github.com/ZackMason/AAGEStarterTemplate

# Features

## Scripting - Native C++

TODO - Examples

## Scripting - Chaiscript

TODO - Examples

TODO - Constants

## Networking

TODO - Examples

## ECS - Entt

AAGE uses entt as an entity component system, but do to the modular structure of the engine it is not required that you actually use it.
The only part of the actual engine that uses it is the scripting virtual machine, which will probably be removed in the near future, when this happens entt will be removed as a dependancy.

## Shader Preprocessing 


### #include

You can include other files that have the `.glsl` extension by using `#include <foo.glsl>`

### Engine constants

You can write to a shader header file called `$ASSET_DIR$/shaders/$FILE_NAME$.glsl` using 
```
static shader_t::add_glsl_constant(const std::string& constant, const std::string& asset_directory, const std::string& file_name) -> void
```

### Vertex Attributes

You can generate vertex attribute using `shader_t::add_attribute_definition`

### Parse uniforms

Shader uniforms can be parsed from shaders by marking them with `// export ?[MIN_RANGE, STEP, MAX_RANGE] ?START_VALUE` and using `shader_t::get_uniform_variables()`.

There is a function to display the uniforms using Imgui called `shader_t::uniform_edit()`

Example
```

uniform float a; // export
uniform int b; // export 1337
uniform int c; // export [0, 5, 1337]
uniform vec3 c1; // export [0, 0.1, 1] 0.5

```

## Scriptable Render Pipeline

TODO - Examples

## Compute Shaders

TODO - Examples

## Cascade Shadow Maps

TODO - Examples

## Events

TODO - Examples

## Asset loader

TODO - Examples

TODO - Resource Handle Example

## Particle Systems

TODO - Examples

TODO - Attractors

## Animations

TODO - Examples

TODO - Blending

## Logging

TODO - Examples

# Examples

## Implementing a Rendering System

TODO

## Multiplayer Pong using batch2d

https://github.com/ZackMason/AAGEPong

## Shader Editor

https://github.com/ZackMason/AAGEShaderEditor

## Voxel Rendering 

https://github.com/ZackMason/AAGEVoxelExample

## Global Illumination

https://github.com/ZackMason/AAGEGlobalIllumination

## Worldmachine Heightmap

TODO

## PhysX Integration

TODO

## Recast Navigation 

TODO

## CastleDB Integration

TODO

# Missing Features

- Audio
- 2D Batch Rendering
- Custom UI
- Inverse Kinematics
- Global Illumination
- G-Buffer
- Forward Plus Utilities
- Tessalation shaders
- Text/Localization 
- DLL exports
- DX11 Backend - probably requires shader abstraction/cross-compilation or user needs to mantain shaders for both APIs
- Emscripten
- Android
- iOS

