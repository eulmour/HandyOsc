file (GLOB FILES_DSP CONFIGURE_DEPENDS
    "*.hpp" "*.cpp"
)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${FILES_DSP}
)