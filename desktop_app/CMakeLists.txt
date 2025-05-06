add_executable(sudokill_desktop_app
    src/main.cpp
)

include_directories(include)

find_package(SDL3)

target_link_libraries(sudokill_desktop_app PRIVATE 
    game_lib 
    SDL3::SDL3
    SDL3_shadercross::SDL3_shadercross
)
