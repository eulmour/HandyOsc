#pragma once
#include "JuceHeader.h"
#include "foleys_gui_magic/foleys_gui_magic.h"

class DynTextItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (DynTextItem)

    DynTextItem(foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
        : foleys::GuiItem (builder, node) {
        
        const auto textColor = node.getProperty("color");
        const auto fontSize = node.getProperty("font-size");
        const auto initText = node.getProperty("text");

        component = std::make_unique<DynText>();
        addAndMakeVisible(component.get());
    }

    ~DynTextItem() {}

    Component* getWrappedComponent() override {
        return this->component.get();
    }

    void update() override {}

protected:

    class DynText : public juce::Component {
    public:
        DynText() {}
        ~DynText() {}

        void paint(Graphics& g) override {
            auto bounds = Rectangle<int> (0, 0, getWidth(), getHeight()).toFloat();
            g.setColour (Colours::white);
            g.drawFittedText (text, bounds.toNearestInt(), Justification::centred, 1);
        }

        void resized() override {}
        void refreshStates() {}
        void setText(juce::String text) { this->text.swapWith(text); }

    protected:
        juce::String text{"Peak: -inf Db"};
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynText)
    };

    std::unique_ptr<DynText> component;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynTextItem)
};

