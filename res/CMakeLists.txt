juce_add_binary_data(BinaryData SOURCES
    gui.xml

    knob.svg
    pointer.svg
    RobotoCondensed-Bold.ttf   
    RobotoCondensed-Regular.ttf   

    presets/Default.mypreset
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
