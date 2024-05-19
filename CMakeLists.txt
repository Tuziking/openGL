cmake_minimum_required(VERSION 3.28)
project(openGL)

set(CMAKE_CXX_STANDARD 17)

# Gather all ImGui resources files
aux_source_directory(${PROJECT_SOURCE_DIR}/include/imgUI IMGUI_SRC)

add_executable(openGL src/glad.c main.cpp ${IMGUI_SRC}
        Shaders/1.1.depth_testing.vs
        "include/GameFrame .h")
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(openGL
        ${PROJECT_SOURCE_DIR}/lib/glfw3.dll
        ${PROJECT_SOURCE_DIR}/lib/libassimp-5.dll)
