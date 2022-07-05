cmake_minimum_required(VERSION 3.12)

project(Ray_Tracer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_IMPLEMENTATIONS glfw opengl3)

include(getGLFW.cmake)
include(getImGui.cmake)
include(getGLM.cmake)
add_subdirectory(deps/glad)

set(SOURCES

    src/main.cpp
    src/application.cpp
    src/window.cpp
    src/window_thread.cpp
    src/frame_buffer.cpp
    src/shader.cpp
    src/gl_error.cpp
    src/render_dispatcher.cpp
    src/renderer.cpp
    src/rt_renderer.cpp
    src/rtmath.cpp
    src/scene.cpp
)

FILE(GLOB HEADERS include/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)

target_link_libraries(${CMAKE_PROJECT_NAME} glfw glad imgui glm)

target_precompile_headers(${CMAKE_PROJECT_NAME} PUBLIC include/rtmath.h)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION .)

install(DIRECTORY resource DESTINATION .)

# add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
# #COMMAND ${CMAKE_COMMAND} -E
# COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resource" $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
# )