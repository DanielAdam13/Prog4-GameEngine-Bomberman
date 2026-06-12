# Link to GitHub repo:
https://github.com/DanielAdam13/Prog4-GameEngine-Bomberman

# Minigin

Minigin is a very small project using [SDL3](https://www.libsdl.org/) and [glm](https://github.com/g-truc/glm) for 2D c++ game projects. It is in no way a game engine, only a barebone start project where everything sdl related has been set up. It contains glm for vector math, to aleviate the need to write custom vector and matrix classes.

[![Build Status](https://github.com/avadae/minigin/actions/workflows/cmake.yml/badge.svg)](https://github.com/avadae/cmake/actions)
[![Build Status](https://github.com/avadae/minigin/actions/workflows/emscripten.yml/badge.svg)](https://github.com/avadae/emscripten/actions)
[![GitHub Release](https://img.shields.io/github/v/release/avadae/minigin?logo=github&sort=semver)](https://github.com/avadae/minigin/releases/latest)

# Goal

Minigin can/may be used as a start project for the exam assignment in the course [Programming 4](https://youtu.be/j96Oh6vzhmg) at DAE. In that assignment students need to recreate a popular 80's arcade game with a game engine they need to program themselves. During the course we discuss several game programming patterns, using the book '[Game Programming Patterns](https://gameprogrammingpatterns.com/)' by [Robert Nystrom](https://github.com/munificent) as reading material. 

# Architectural Rules

- Commands are intent, Components are behavior
- Mutatation during iteation is bad - defer to a later point (Mark For Delete, Pending Additions)
- No Component can add/remove other components from its owner internally
- Class for state and behavior / Namespace for stateless logic
- Data lives with the system that owns it (Stage descriptors -> stage Loader, Camera -> Gameplay State)
- Tie a resource's lifetime to an object's lifetime - apply RAII (Obsever auto-unsubscribe, no raw pointers for ownership)

---

# Used Design Patterns

### Component

    A Component is implemented "Not to reuse, but to be reused"

A Game Object OWNS a container of Components, not allowing any duplicates. The Game Object is the top-most level of abstraction in the game engine, it is the "main hub" for any methods/variables. Components, Commands, States all receive information via the Game Object target/owner. The feature of a Game Object not having any duplicate Components is very beneficial for a lot of reasons:
- It makes cross-component lookup very CONVENIENT - you just call GetComponent<T> (where T is the component type) from anywhere and the user is sure that they are receiving only a singular instance, no overcomplication in the regard of wondering "which one".
- Observers are all built on the assumption that only one Component of a type exists on a Game Object. If it didn't, the event logic would become entangled and complex.
- State coherence - if a Game Object has multiple Health Components, what does that even mean?

### Command

    Specify intent, not behavior
The Command Pattern is entirely separate from the Components. A Command will receive input and change values on the target Game Object but then a Component can do its own logic despite the command, making them, overall, a separate abstraction. The command's job is to encapsulate / "wrap" a function call and that's their main benefit. They do a very specific thing while being purely a call abstraction. 

Implemented in a clean layered design:
- Base abstraction (Command) -> allows Execute() overriding
- Game Object targetting (GameObjectCommand) -> base for command targetting specific Game Objects. Stores a non-owning reference so commands can be rebound.
- Game-flow commands (SwitchToMainMenuCommand...) -> act on game-wide state, override directly from base Command.
- UI Commands (HoverSelectableCommand) -> delegate to methods on a state. All hover/selection logic lives on the state, not in the command.

### Observer

    Decopuling identity - a Subject notifies an Obsever in the same call stack as the event source
Notifying that something has happened and not caring about who listens is a big deal in this project's architecture. 
A bomb explosion does not care about what sound is played or what is created from it, it only tells the rest of the program that THIS has happened.

### State

    Abstract setup, teardown, "What's next" logic outside of the update loop. The machine is the dispatcher and applies pending transitions
Fully utilized in the Game State Machine - drives the whole MainMenu -> Gameplay -> GameOver flow:

![Research image](/resources/readme/GameStateMachine.png?raw=true "Game State Machine") Game State Machine

Applied to the Enemy AI as well - EnemyComponent + Enemy State. Implements per-instance behavior inside one component while using deferred transitioning.


### Update Method, Game Loop

    Simulating a collection of objects, one frame at a time while caring about Destruction/Additiong mid-iteration
This is basically the core loop of everything. 
A Scene Manager is Updated once a frame -> Calls Update for every Scene -> Calls Update for every Game Object -> Calls Update for every Component.

### Flyweight

    The engine applies half of it - memory sharing between object instances, SDL handles shared state for performance
The whole premise behind the ResourceManager is all resources having a shared place of ownership but MORE IMPORTANTLY - reusability and caching.
Textures, Spite Sheets, Fonts all benefit greatly from this.

### Dirty Flag

    Deffering an expensive calculation - Transform
The Transform's Component core idea - Marking Dirty ONLY when a recalculation is needed, not every frame. This means Setting the position, rotation or scale all 
mark the flag and Getting them does the Recalculation. This applies recursively to every CHILD of a Game Object as well.

### Singleton

    Ensure a class has one instance and provide a global point of access to it
Used in classes like ResourceManager, Renderer, GameEngine, the CollisionSystem. It is ok to expose these classes to every point since they are too high-level and would be
annoying to pass as a parameter for every call site - this would become TOO tedious, 100+ places in my code so far.

### Service Locator

    Tries solving the singleton issue by providing a shared point of access to things that NEED one instance
Applied for the Sound System and Input Manager. Initialized with a Null version until a derived service is assigned.

### Type Object

    Build a dynamic data-based object from a static class-based variation
Insted of subclassing every variation, we expose one class for the live thing and another for what TYPE it is.
Exact mapping in code:
- Typed object: EnemyComponent (dynamic instance). Has health, position, state machine, movement...
- Type object: EnemyArchetype (shared per-type data). SpriteSheet, speed, detection radius...

### Subclass Sandbox

    Providing a "sandbox" of available operations in a base class. Dervied classes reuse them.
The Enemy State base class provides protected helpers - FindClosestPlayerInRange, CollectWalkableNeighbors, GetSourceEnemyComponent, GetSourceTransform.
These are all reused in every derived Enemy State.

# Bomberman Architectural Choices

### The Bomb Laying problem

Where should a bomb be created? All Update logic happens in Components and it is indeed a separate "mechanic", so in a Bomb Component? Should it be in the Player Component? Maybe the game could store a "prototype" bomb Game Object? All of these are bad ideas.

A player would need to PRESS a button in order to lay a bomb. That means that it is intuitive to do it in a Lay Bomb Command. The bomb can be created once the Execute method fires.
Now comes the problem, WHEN should we spawn a bomb? The Bomberman game does not allow two (without powerups) bombs to exists at the same time (for a single player). As explained in the Commands section above, this Lay Bomb Command is very specific, meaning that we SHOULD evaluate the Can Spawn Bomb condition INSIDE its Execute method. 

I decided to solve this issue by creating a specific Component for laying a bomb which basically tracks the active bombs. It increments when a bomb is REGISTERED and decrements by listening/OBSERVING the Bomb_Explosion Game event.

### Enemy AI
An enemy needs to walk in the grid based level. For that, logic for movement to Tile Target is implemented. 
Movement setting happens in Enemy Component, actual direction choice is done in the States + the state switching still happens inside the States themselves.

As before, setting the Position of the enemy Game Object happens from Enemy Component with it now including the Move to Tile logic.
How the directions are calculated happens from the States via a virtual method ChooseDirectionAtIntersection.
This method is then called from the Enemy Component when a target tile is reached.

The enemy is moving between centers of the tile.
When a target is reached, the position is clamped to the mid point to prevent overshooting.

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

