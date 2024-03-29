# Lua-Engine

LuaEngine is a simple 2D game engine written in C. 

# Features

It's main feature is an extensive use of Lua programming language and it's features. 
For example every scene used by engine must be Lua module. 
It's also provides some basic features for 2d games like: 
* rendering and animating images of PNG, JPEG, BMP and TIFF formats; 
* playing WAV and OGG audio files; 
* object model for speeding up development process.


# Documentation

LuaEngine structure

LuaEngine consists of main executable and numerous modules written in Lua C API and loaded during executable start
There's the list of this modules:
  * Graphics - module for drawing various drawable objects(mainly Text, Sprite) and primitives(mainly Square, Quad and Circle). Can change color of drawn objects if module's 'color' field changes.
  * Audio - sound module for playing audio of types .ogg and .wav. Has capabilities for relative sound position.
  * Collision - intersection module for checking figures collision. Supports intersection checks between circles, squares and points.

LuaEngine uses object model for ease of use.
There are several classes that LuaEngine uses:
  * Sprite - drawable object, can be either Single image or animated SpriteSheet.
  * Text - drawable object, consists of the font and it's size.
  * Mob - movable object, implements number of common functionality for moving 2D objects.
  * Layer - drawing object, stores drawables inside.
  * Color - basic data type for color handling.
  * Vector - simple class for vector math.

## Class Mixing
  In Lua there is no object-oriented model. However, it's easy to implement it. One of the ways to implement objects in 
  Lua is to create a mixin-table that represents your object and consits of it's fields summed up with the values of it's
  parent object.

# Dependencies

IUP 3.29, 
lua 5.4,
pthread,
FreeType,
dr_wav,
stb_vorbis,
OpenAL,
OpenGL


# Building advice 

makefile does everything for your
