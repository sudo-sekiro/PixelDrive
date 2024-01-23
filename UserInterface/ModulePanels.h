#pragma once

#ifndef USERINTERFACE_MODULEPANELS_H_
#define USERINTERFACE_MODULEPANELS_H_

#include <JuceHeader.h>

#include "CustomSlider.h"
#include "CustomToggle.h"

#include <memory>
#include <vector>

#define HEADER_SIZE 30

// UI component for choosing and creating user presets
class DistortionPanel : public Component {
 public:
    CustomRotarySlider distortionPreGainSlider, distortionToneSlider, distortionPostGainSlider,
        distortionClaritySlider;
    CustomToggleButton distortionBypassButton{"Bypass"};
    DistortionPanel::DistortionPanel() {
        // Distortion labels
        distortionPreGainSlider.addSliderLabels("-10dB", "100dB", "Input Gain");
        distortionToneSlider.addSliderLabels("0", "10", "Tone");
        distortionPostGainSlider.addSliderLabels("-24dB", "24dB", "Output Gain");
        distortionClaritySlider.addSliderLabels("0", "10", "Clarity");
        // distortionBypassButton
        for (auto* comp : getComps()) {
            addAndMakeVisible(comp);
        }
    }

    DistortionPanel::~DistortionPanel() {}

    std::vector<juce::Component*> getComps() {
        return {
            &distortionPreGainSlider, &distortionToneSlider, &distortionPostGainSlider,
            &distortionClaritySlider, &distortionBypassButton
        };
    }

    void DistortionPanel::resized() override {
        const auto container = getLocalBounds().reduced(4);
        auto distortionBounds = container;

        // Distortion top row
        auto title = distortionBounds.removeFromTop(distortionBounds.getHeight() / 4);
        auto distortionBoundsTop = distortionBounds.removeFromTop(distortionBounds.getHeight() / 4);
        distortionToneSlider.setBounds(distortionBoundsTop.removeFromLeft(distortionBoundsTop.getWidth() / 3));
        distortionBypassButton.setBounds(distortionBoundsTop.removeFromLeft(distortionBoundsTop.getWidth() / 2));
        distortionClaritySlider.setBounds(distortionBoundsTop);
        // Distortion bottom row
        distortionPostGainSlider.setBounds(distortionBounds.removeFromLeft(distortionBounds.getWidth() / 2));
        distortionPreGainSlider.setBounds(distortionBounds);
    }

    void DistortionPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.getHeight() / 4);
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("distortion", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    // Distortion slider
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionPanel);
};

#endif  // USERINTERFACE_MODULEPANELS_H_
