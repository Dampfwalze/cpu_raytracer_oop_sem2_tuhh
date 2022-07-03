include(FetchContent)

FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
)

FetchContent_MakeAvailable(glm)

# glm uses volatile at one point, which is deprecated by c++ standard
target_compile_options(glm INTERFACE -Wno-volatile)