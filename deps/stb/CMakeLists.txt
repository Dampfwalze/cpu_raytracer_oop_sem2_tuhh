
option(STBI_FAILURE_USERMSG off)

option(STBI_NO_JPEG off)
option(STBI_NO_PNG off)
option(STBI_NO_BMP off)
option(STBI_NO_PSD off)
option(STBI_NO_TGA off)
option(STBI_NO_GIF off)
option(STBI_NO_HDR off)
option(STBI_NO_PIC off)
option(STBI_NO_PNM off)

option(STBI_ONLY_JPEG off)
option(STBI_ONLY_PNG off)
option(STBI_ONLY_BMP off)
option(STBI_ONLY_PSD off)
option(STBI_ONLY_TGA off)
option(STBI_ONLY_GIF off)
option(STBI_ONLY_HDR off)
option(STBI_ONLY_PIC off)
option(STBI_ONLY_PNM off)

option(STBI_SUPPORT_ZLIB off)

option(STBI_WINDOWS_UTF8 off)

configure_file("stb_config.h.in" "stb_config.h")

add_library(stb_image OBJECT stb_image.cpp)

target_include_directories(stb_image PUBLIC include "${CMAKE_CURRENT_BINARY_DIR}")