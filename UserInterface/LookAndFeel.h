#ifndef USERINTERFACE_LOOKANDFEEL_H_
#define USERINTERFACE_LOOKANDFEEL_H_

struct CustomLookAndFeel : juce::LookAndFeel_V4 {
    void drawRotarySlider(juce::Graphics& g,
                                   int x, int y, int width, int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider& slider) override;

    void drawToggleButton(juce::Graphics& g,
                                   juce::ToggleButton& toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown) override;

    int getTextHeight() const {return 14; }
};

#endif  // USERINTERFACE_LOOKANDFEEL_H_
