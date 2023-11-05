#ifndef __USER_INTERFACE__CUSTOMSLIDER_
#define __USER_INTERFACE__CUSTOMSLIDER_

#include "LookAndFeel.h"
struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                     juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        setLookAndFeel(&lnf);
    }
    ~CustomRotarySlider()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };
    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const {return 14; }
    juce::String getDisplayString() const;
    void addSliderLabels(juce::String minLabel, juce::String maxLabel, juce::String titleLabel);
private:
    LookAndFeel lnf;
};

#endif // __USER_INTERFACE__CUSTOMSLIDER_