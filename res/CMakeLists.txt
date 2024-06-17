juce_add_binary_data(BinaryData SOURCES
    presets/Default.mypreset
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
