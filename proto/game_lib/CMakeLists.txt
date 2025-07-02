add_library(game_lib STATIC
    src/game.cpp
)

target_include_directories(game_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
