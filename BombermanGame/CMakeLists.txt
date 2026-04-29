#----------------------------
# Game executable — Bomberman
#----------------------------
add_executable(${TARGET_NAME} ${TARGET_PARAM}
  "Main.cpp"
  "GameEvents.h"
  "Achievements.h" "Achievements.cpp"
 
  "Components/PlayerComponent.h"        "Components/PlayerComponent.cpp"
  "Components/HealthDisplayComponent.h" "Components/HealthDisplayComponent.cpp"
  "Components/ScoreDisplayComponent.h"  "Components/ScoreDisplayComponent.cpp"
 
  "Commands/DamageCommand.h" "Commands/DamageCommand.cpp"
  "Commands/ScoreCommand.h"  "Commands/ScoreCommand.cpp"
  "Services/SoundIds.h"
  "Services/SDLSoundSystem.h" "Services/SDLSoundSystem.cpp" 
  "BombermanGame.h" "BombermanGame.cpp" "SoundManager.h" "SoundManager.cpp")
 
target_include_directories(${TARGET_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
 
# Linking the engine pulls in SDL3, SDL3_ttf, glm, imgui, VLD, xinput
target_link_libraries(${TARGET_NAME} PRIVATE gameEngine_lib)

target_link_libraries(${TARGET_NAME} PRIVATE SDL3_mixer::SDL3_mixer)

ge_apply_warnings(${TARGET_NAME})

#----------------------------
# Steamworks (only links to the executable, not the engine library).
#----------------------------
if(Steamworks_FOUND)
  target_link_libraries(${TARGET_NAME} PRIVATE Steamworks::Steamworks)
  target_compile_definitions(${TARGET_NAME} PRIVATE USE_STEAMWORKS)
endif()
 
#----------------------------
# Platform-specific post-build steps.
#----------------------------
if(EMSCRIPTEN)
  set(EMSCRIPTEN_COMPILE_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -g")
  set(EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/resources@/\"")
 
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/EmscriptenToolchain.cmake)
 
  set_target_properties(${TARGET_NAME} PROPERTIES
      COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
  )
 
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/web" "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
  )
 
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
 
elseif(WIN32)
  set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
  set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>")
 
  set(SDL_RUNTIME_DLLS
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
  )
  foreach(dllTarget IN LISTS SDL_RUNTIME_DLLS)
    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${dllTarget}>"
          "$<TARGET_FILE_DIR:${TARGET_NAME}>"
      VERBATIM
    )
  endforeach()
 
  if(Steamworks_FOUND)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy "${Steamworks_RUNTIME_DLL}" "$<TARGET_FILE_DIR:${TARGET_NAME}>")
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt"
      COMMENT "Generating steam_appid.txt"
    )
  endif()
 
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources"
  )
elseif(APPLE)
  set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3-shared>" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
    COMMAND install_name_tool -change "@rpath/libSDL3_ttf.0.dylib" "@executable_path/../Frameworks/libSDL3_ttf.0.2.2.dylib" "$<TARGET_FILE:${TARGET_NAME}>"
    COMMAND install_name_tool -change "@rpath/libSDL3.0.dylib" "@executable_path/../Frameworks/libSDL3.0.dylib" "$<TARGET_FILE:${TARGET_NAME}>"
  )
 
  if(Steamworks_FOUND)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${Steamworks_LIBRARIES}" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
    )
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt"
      COMMENT "Generating steam_appid.txt"
    )
  endif()
 
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources"
  )
else()
  if(Steamworks_FOUND)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${Steamworks_LIBRARIES}" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
    )
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt"
      COMMENT "Generating steam_appid.txt"
    )
  endif()
 
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources"
  )
endif()
