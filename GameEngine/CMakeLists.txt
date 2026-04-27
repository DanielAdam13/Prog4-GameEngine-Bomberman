#----------------------------
# GameEngine — static library
#----------------------------
add_library(gameEngine_lib STATIC
  "GameEngine.h" "GameEngine.cpp"
  "Singleton.h"
  "Renderer.h" "Renderer.cpp"
  "Texture2D.h" "Texture2D.cpp"
  "ResourceManager.h" "ResourceManager.cpp"
  "Font.h" "Font.cpp"
  "Services/InputManager.h" "Services/InputManager.cpp"
  "Scene.h" "Scene.cpp"
  "SceneManager.h" "SceneManager.cpp"
  "GameObject.h" "GameObject.cpp"
  "ImGuiScene.h" "ImGuiScene.cpp"
  "Observer.h"
  "ObservableSubject.h" "ObservableSubject.cpp"
  "Services/ServiceLocator.h" "Services/ServiceLocator.cpp"
  "Services/SoundSystem.h" "Services/SoundSystem.cpp"
  "Services/NullSoundSystem.h"
  "Services/SDLSoundSystem.h" "Services/SDLSoundSystem.cpp"
  "Services/LoggingSoundService.h" "Services/LoggingSoundService.cpp"
 
  "Components/Component.h"
  "Components/Transform.h" "Components/Transform.cpp"
  "Components/Image.h" "Components/Image.cpp"
  "Components/TextComponent.h" "Components/TextComponent.cpp"
  "Components/FPSComponent.h" "Components/FPSComponent.cpp"
  "Components/RotatorComponent.h" "Components/RotatorComponent.cpp"
  "Components/HealthComponent.h" "Components/HealthComponent.cpp"
  "Components/ScoreComponent.h" "Components/ScoreComponent.cpp"
 
  "Commands/Command.h"
  "Commands/GameObjectCommand.h"
  "Commands/MoveCommand.h" "Commands/MoveCommand.cpp"
  "Commands/MoveStickCommand.h" "Commands/MoveStickCommand.cpp"
  "Commands/ConditionalCommand.h" "Commands/ConditionalCommand.cpp" 
)

target_compile_features(gameEngine_lib PUBLIC cxx_std_20)

target_include_directories(gameEngine_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Dependencies — PUBLIC because public engine headers leak them into consumers
target_link_libraries(gameEngine_lib PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  glm::glm
  imgui
)

# VLD
if(WIN32 AND VLD_FOUND)
  target_include_directories(gameEngine_lib PUBLIC ${VLD_INCLUDE_DIR})
  target_link_libraries(gameEngine_lib PUBLIC ${VLD_LIBRARIES})
endif()

# XInput
target_link_libraries(gameEngine_lib PUBLIC
  $<$<PLATFORM_ID:Windows>:xinput.lib>
)

ge_apply_warnings(gameEngine_lib)