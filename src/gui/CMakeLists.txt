file (GLOB FILES_GUI CONFIGURE_DEPENDS
    "*.hpp" "*.cpp"
    "item/*.hpp" "item/*.cpp"
    "lnf/*.hpp" "lnf/*.cpp"
)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${FILES_GUI}
)