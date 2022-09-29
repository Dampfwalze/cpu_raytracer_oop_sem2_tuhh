include(FetchContent)

FetchContent_Declare(
    nfd
    GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog.git
    GIT_TAG master
)

FetchContent_MakeAvailable(nfd)

set(nfd_SOURCE_FILES "${nfd_SOURCE_DIR}/src/nfd_common.c")

if(WIN32)
    list(APPEND nfd_SOURCE_FILES "${nfd_SOURCE_DIR}/src/nfd_win.cpp")
endif()

if(APPLE)
    list(APPEND nfd_SOURCE_FILES "${nfd_SOURCE_DIR}/src/nfd_cocoa.m")
endif()

if(UNIX AND NOT APPLE)
    list(APPEND nfd_SOURCE_FILES "${nfd_SOURCE_DIR}/src/nfd_zenity.c")
endif()

add_library(nfd OBJECT ${nfd_SOURCE_FILES})

target_include_directories(nfd PUBLIC "${nfd_SOURCE_DIR}/src/include")