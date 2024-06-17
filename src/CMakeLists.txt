option(BUILD_HEADLESS "Build WarpRoom headless utilities" OFF)
if(NOT (IOS OR MACOS_RELEASE) AND BUILD_HEADLESS)
    message(STATUS "Building WarpRoom Headless!")
    add_subdirectory(headless)
endif()

file (GLOB FILES_SRC CONFIGURE_DEPENDS "*.h" "*.cpp")

# target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "gui")

target_sources(WarpRoom PRIVATE ${FILES_SRC})

target_precompile_headers(WarpRoom PRIVATE pch.h)

add_subdirectory("dsp")
add_subdirectory("gui")
