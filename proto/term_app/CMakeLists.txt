add_executable(sudokill_term_app
    src/main.cpp
)

include_directories(include)

target_link_libraries(sudokill_term_app PRIVATE game_lib)
