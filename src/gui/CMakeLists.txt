file (GLOB FILES_GUI CONFIGURE_DEPENDS
    "*.h" "*.cpp"
)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${FILES_GUI}
)