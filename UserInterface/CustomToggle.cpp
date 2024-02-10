#include "UserInterface/CustomToggle.h"

#define TOGGLE_LABEL_COLOUR juce::Colours::black

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g,
                                   juce::ToggleButton& toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown) {
    using juce::Rectangle, juce::ignoreUnused, juce::Colour, juce::jmin;
    ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    juce::Image toggleBackground;

    auto bounds = toggleButton.getLocalBounds();
    // g.drawRect(bounds);

    auto textBox = bounds.removeFromTop(getTextHeight());
    auto str = toggleButton.getButtonText();
    g.setColour(TOGGLE_LABEL_COLOUR);
    g.drawFittedText(str, textBox.toNearestInt(), juce::Justification::centred, 1);

    auto size = jmin(bounds.getWidth(), bounds.getHeight());
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
    // g.drawRect(r);

    if (!toggleButton.getToggleState()) {
        // Use on state toggle background
        if (toggleButton.isDown()) {
            toggleBackground = ImageFileFormat::loadFrom(BinaryData::toggle_on_down_png,
                                                         BinaryData::toggle_on_down_pngSize);
        } else {
            toggleBackground = ImageFileFormat::loadFrom(BinaryData::toggle_button_on_png,
                                                         BinaryData::toggle_button_on_pngSize);
        }
    } else {
        // Use off state toggle background
        if (toggleButton.isDown()) {
            toggleBackground = ImageFileFormat::loadFrom(BinaryData::toggle_off_down_png,
                                                         BinaryData::toggle_off_down_pngSize);
        } else {
            toggleBackground = ImageFileFormat::loadFrom(BinaryData::toggle_button_off_png,
                                                         BinaryData::toggle_button_off_pngSize);
        }
    }

    g.drawImage(toggleBackground,  // Image to draw
                r.toFloat(),  // Rectangle to draw within
                juce::RectanglePlacement::fillDestination,  // placementWithinTarget - default stretchToFit
                false);  // fillAlphaChannelWithCurrentBrush
}
