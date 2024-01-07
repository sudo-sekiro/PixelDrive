#include "UserInterface/CustomToggle.h"

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g,
                                   juce::ToggleButton& toggleButton,
                                   bool shouldDrawButtonAsHighlighted,
                                   bool shouldDrawButtonAsDown) {
    using juce::Rectangle, juce::ignoreUnused, juce::Colour, juce::jmin;
    ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    auto bounds = toggleButton.getLocalBounds();
    // g.drawRect(bounds);

    auto textBox = bounds.removeFromTop(getTextHeight());
    auto str = toggleButton.getButtonText();
    g.setColour(Colour(0u, 172u, 1u));
    g.drawFittedText(str, textBox.toNearestInt(), juce::Justification::centred, 1);

    auto size = jmin(bounds.getWidth(), bounds.getHeight());
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
    // g.drawRect(r);

    Rectangle<float> base = r.withSizeKeepingCentre(size / 1.5f, size / 4.5f).toFloat();
    base.setY(r.getCentreY());

    auto buttonHeight = toggleButton.getToggleState() ?  size / 5 : size / 10;
    Rectangle<float> cylinder = r.withSizeKeepingCentre(size / 2.f, static_cast<float>(buttonHeight)).toFloat();
    cylinder.setBottom(base.getCentreY());
    Rectangle<float> cylinderTop = cylinder.withTrimmedBottom(cylinder.getHeight() / 2);

    auto colour = toggleButton.getToggleState() ?  juce::Colours::dimgrey : Colour(0u, 172u, 1u);
    g.setColour(colour);
    g.fillEllipse(base);
    g.setColour(juce::Colours::black);
    g.drawEllipse(base, 2.f);

    g.setColour(juce::Colours::silver);
    g.fillRoundedRectangle(cylinder, size / 8.f);
    g.setColour(juce::Colours::black);
    g.drawEllipse(cylinderTop, 2.f);
}
