#ifndef __USER_INTERFACE__LOOKANDFEEL_
#define __USER_INTERFACE__LOOKANDFEEL_

struct LookAndFeel : juce::LookAndFeel_V4
{
    virtual void drawRotarySlider (juce::Graphics& g,
                                   int x, int y, int width, int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider& slider) override;

    virtual void drawToggleButton (juce::Graphics& g,
                                   juce::ToggleButton& toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown) override;
};

#endif //__USER_INTERFACE__LOOKANDFEEL_