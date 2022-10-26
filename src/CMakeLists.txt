option(BUILD_HEADLESS "Build HandyOsc headless utilities" OFF)
if(NOT (IOS OR MACOS_RELEASE) AND BUILD_HEADLESS)
    message(STATUS "Building HandyOsc Headless!")
    add_subdirectory(headless)
endif()

file (GLOB FILES_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

# target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "gui")

target_sources(HandyOsc PRIVATE ${FILES_SRC})

target_precompile_headers(HandyOsc PRIVATE pch.h)

add_subdirectory("dsp")
add_subdirectory("gui")
