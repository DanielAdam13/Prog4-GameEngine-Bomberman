# Minigin

Minigin is a very small project using [SDL3](https://www.libsdl.org/) and [glm](https://github.com/g-truc/glm) for 2D c++ game projects. It is in no way a game engine, only a barebone start project where everything sdl related has been set up. It contains glm for vector math, to aleviate the need to write custom vector and matrix classes.

[![Build Status](https://github.com/avadae/minigin/actions/workflows/cmake.yml/badge.svg)](https://github.com/avadae/cmake/actions)
[![Build Status](https://github.com/avadae/minigin/actions/workflows/emscripten.yml/badge.svg)](https://github.com/avadae/emscripten/actions)
[![GitHub Release](https://img.shields.io/github/v/release/avadae/minigin?logo=github&sort=semver)](https://github.com/avadae/minigin/releases/latest)

# Goal

Minigin can/may be used as a start project for the exam assignment in the course [Programming 4](https://youtu.be/j96Oh6vzhmg) at DAE. In that assignment students need to recreate a popular 80's arcade game with a game engine they need to program themselves. During the course we discuss several game programming patterns, using the book '[Game Programming Patterns](https://gameprogrammingpatterns.com/)' by [Robert Nystrom](https://github.com/munificent) as reading material. 

# Architectural Choices

Game Object <-> Component
A Game Object OWNS a container of Components, not allowing any duplicates. The Game Object is the top-most level of abstraction in the game engine, it is the "main hub" for any methods/variables. Components, Commands, States all receive information via the Game Object target/owner. The feature of a Game Object not having any duplicate Components is very beneficial for a lot of reasons:
- It makes cross-component lookup very CONVENIENT - you just call GetComponent<T> (where T is the component type) from anywhere and the user is sure that they are receiving only a singular instance, no overcomplication in the regard of wondering "which one".
- Observers are all built on the assumption that only one Component of a type exists on a Game Object. If it didn't, the event logic would become entangled and complex.
- State coherence - if a Game Object has multiple Health Components, what does that even mean? It makes no sense.

Component <-> Command
The Component system is entirely separate from the Command System. A Command will receive input and may change values on the target Game Object but then a Component can do its own logic despite the command, making them, overall, a separate abstraction.

State <-> Component, Command
In the same way, a State is a yet another level of abstraction, different from both a Component and Command. A State will receive a potentially already changed references to variables from the Game Object itself, which might already be updated by a Command or a Component.

States, Transitions:
States are a feature in the game engine that could be used via the FSM (Finite State Machine) Component. The FSMComponent OWNS all states and transitions. It does not care about any outside information, it only changes the current state by EVALUATING its transitions. As mentioned above, this component benefits greatly from the single instance feature for a Component on a Game Object - for a Game Object, it makes sense that it only has one State Machine.


## Windows version

Either
- Open the root folder in Visual Studio 2026; this will be recognized as a cmake project.
  
Or
- Install CMake 
- Install CMake and CMake Tools extensions in Visual Code
- Open the root folder in Visual Code,  this will be recognized as a cmake project.

Or
- Use whatever editor you like :)

## Emscripten (web) version

### On windows

For installing all of the needed tools on Windows I recommend using [Chocolatey](https://chocolatey.org/). You can then run the following in a terminal to install what is needed:

    choco install -y cmake
    choco install -y emscripten
    choco install -y ninja
    choco install -y python

In a terminal, navigate to the root folder. Run this: 

    mkdir build_web
    cd build_web
    emcmake cmake ..
    emmake ninja

To be able to see the webpage you can start a python webserver in the build_web folder

    python -m http.server

Then browse to http://localhost:8000 and you're good to go.

### On OSX

On Mac you can use homebrew

    brew install cmake
    brew install emscripten
    brew install python

In a terminal on OSX, navigate to the root folder. Run this: 

    mkdir build_web
    cd build_web
    emcmake cmake .. -DCMAKE_OSX_ARCHITECTURES=""
    emmake make

To be able to see the webpage you can start a python webserver in the build_web folder

    python3 -m http.server

Then browse to http://localhost:8000 and you're good to go.

## Github Actions

This project is build with github actions.
- The CMake workflow builds the project in Debug and Release for Windows and serves as a check that the project builds on that platform.
- The Emscripten workflow generates a web version of the project and publishes it as a [github page](https://avadae.github.io/minigin/). 
  - The url of that page will be `https://<username>.github.io/<repository>/`
- You can embed this page with 

```<iframe style="position: absolute; top: 0px; left: 0px; width: 1024px; height: 576px;" src="https://<username>.github.io/<repository>/" loading="lazy"></iframe>```

