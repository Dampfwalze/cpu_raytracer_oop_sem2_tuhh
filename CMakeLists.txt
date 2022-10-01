cmake_minimum_required(VERSION 3.12)

option(BUILD_DOC "Build documentation" ON)

project(Ray_Tracer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_IMPLEMENTATIONS glfw opengl3)

include(getGLFW.cmake)
include(getImGui.cmake)
include(getGLM.cmake)
include(getYAML-cpp.cmake)
include(getNativeFileDialog.cmake)
add_subdirectory(deps/glad)
add_subdirectory(deps/stb)
add_subdirectory(deps/tclap)

set(SOURCES

    src/main.cpp
    src/application.cpp
    src/window.cpp
    src/window_thread.cpp
    src/frame_buffer.cpp
    src/shader.cpp
    src/gl_error.cpp
    src/render_thread.cpp
    src/renderer.cpp
    src/rt_renderer.cpp
    src/rtmath.cpp
    src/scene.cpp
    src/transform.cpp
    src/scene_object.cpp
    src/scene_shapes.cpp
    src/scene_lights.cpp
    src/sampler.cpp
    src/camera.cpp
    src/material.cpp
    src/pixel_logger.cpp
    src/profiler.cpp
    src/resource_container.cpp
    src/resource_loaders.cpp
    src/scene_deserializer.cpp
    src/scene_serializer.cpp
    src/file_dialog.cpp
)

FILE(GLOB_RECURSE HEADERS include/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)

target_link_libraries(${CMAKE_PROJECT_NAME} glfw glad imgui glm stb_image yaml-cpp nfd tclap)

target_precompile_headers(${CMAKE_PROJECT_NAME} PUBLIC include/rtmath.h)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION .)

install(DIRECTORY resource DESTINATION .)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resource" $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/resource
)

# documentation
if(BUILD_DOC)
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in" "Doxyfile" @ONLY)

        add_custom_target(doc_doxygen ALL
            COMMAND "${DOXYGEN_EXECUTABLE}" "Doxyfile"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    else(DOXYGEN_FOUND)
        message("Doxygen needs to be installed to generate the doxygen documentation (https://doxygen.nl/download.html)")
    endif()
endif()