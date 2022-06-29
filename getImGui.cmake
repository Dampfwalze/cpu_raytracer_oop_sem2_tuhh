include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)

FetchContent_MakeAvailable(imgui)

FILE(GLOB imgui_SOURCES "${imgui_SOURCE_DIR}/*.cpp")

foreach(impl ${IMGUI_IMPLEMENTATIONS})
    list(APPEND imgui_SOURCES "${imgui_SOURCE_DIR}/backends/imgui_impl_${impl}.cpp")
endforeach()

add_library(imgui OBJECT "${imgui_SOURCES}")

target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")

target_link_libraries(imgui PRIVATE glfw)