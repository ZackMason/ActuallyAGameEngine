# ActuallyAGameEngine

![image](https://user-images.githubusercontent.com/3623261/184268773-de19e916-4889-450d-9553-8c058959e6c0.png)

# Why AAGE?

AAGE is designed to be modular, and easy to modify. AAGE provides easy to use abstractions for OpenGL, Networking, Scripting, and more.
AAGE is meant to be a less opinionated alternative to raylib. The main use case of this engine is for creating 3D games, although other types of applications should be possible.

# Template

You can find a start template at https://github.com/ZackMason/AAGEStarterTemplate

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

## Compute Shaders

TODO - Examples

## Cascade Shadow Maps

TODO - Examples

## Events

TODO - Examples

## Asset loader

TODO - Examples

## Animations

TODO - Examples
TODO - Blending

# Examples

## Worldmachine Heightmap

TODO

## PhysX Integration

TODO




