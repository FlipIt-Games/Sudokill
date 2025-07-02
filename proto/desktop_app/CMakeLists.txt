add_executable(sudokill_desktop_app
    src/main.cpp
)

include_directories(include)
include_directories(${PROJECT_NAME} PRIVATE 
    ${SDL3_include} 
    ${SDL3_shadercross_include}
    ${SDL3_ttf_include}
)

target_link_libraries(sudokill_desktop_app PRIVATE 
    game_lib 
    ${SDL3_lib} 
    ${SDL3_shadercross_lib}
    ${SDL3_ttf_lib}
)

target_compile_definitions(sudokill_desktop_app PRIVATE
    RES_FOLDER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res"
)
