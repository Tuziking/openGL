cmake_minimum_required(VERSION 3.28)
project(openGL)

set(CMAKE_CXX_STANDARD 17)

# Gather all ImGui resources files
aux_source_directory(${PROJECT_SOURCE_DIR}/include/imgUI IMGUI_SRC)

add_executable(openGL
        src/glad.c 
        main.cpp
        ${IMGUI_SRC}
        src/Particle.cpp
)
#add_executable(test_program
#        src/glad.c
#        test/main.cpp
#        src/Particle.cpp
#)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 添加glfw3.lib
target_link_libraries(openGL
        ${PROJECT_SOURCE_DIR}/lib/glfw3.dll
        ${PROJECT_SOURCE_DIR}/lib/libassimp-5.dll)

## 让test/main。cpp链接到glfw3.dll
#target_link_libraries(test_program
#        ${PROJECT_SOURCE_DIR}/lib/glfw3.dll
#        ${PROJECT_SOURCE_DIR}/lib/libassimp-5.dll)



