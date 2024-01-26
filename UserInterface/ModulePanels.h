#pragma once

#ifndef USERINTERFACE_MODULEPANELS_H_
#define USERINTERFACE_MODULEPANELS_H_

#include <JuceHeader.h>

#include "CustomSlider.h"
#include "CustomToggle.h"

#include <memory>
#include <vector>

#define HEADER_SIZE 30

// UI component for the distortion pedal
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionPanel);
};

// UI component for the amplifier simulator
class AmpPanel : public Component {
 public:
    // Amp sliders
    CustomRotarySlider ampInputGainSlider, ampLowEndSlider, ampMidsSlider, ampHighEndSlider;
    CustomToggleButton ampBypassButton{"Bypass"};
    AmpPanel::AmpPanel() {
        // Amp labels
        ampInputGainSlider.addSliderLabels("0", "11", "Input Gain");
        ampLowEndSlider.addSliderLabels("0", "11", "Bass");
        ampMidsSlider.addSliderLabels("0", "11", "Mids");
        ampHighEndSlider.addSliderLabels("0", "11", "Treble");
        for (auto* comp : getComps()) {
            addAndMakeVisible(comp);
        }
    }

    AmpPanel::~AmpPanel() {}

    std::vector<juce::Component*> getComps() {
        return {
            &ampInputGainSlider, &ampLowEndSlider, &ampMidsSlider, &ampHighEndSlider,
            &ampBypassButton
        };
    }

    void AmpPanel::resized() override {
        const auto container = getLocalBounds();
        auto ampBounds = container;

        // Title text
        ampBounds.removeFromTop(ampBounds.getHeight() / 4);

        // Add amp sliders
        ampInputGainSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 5));
        ampLowEndSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 4));
        ampMidsSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 3));
        ampHighEndSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 2));
        ampBypassButton.setBounds(ampBounds);
    }

    void AmpPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.getHeight() / 4);
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("Amp", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpPanel);
};

// UI component for the delay module
class DelayPanel : public Component {
 public:
    // Delay sliders
    CustomRotarySlider delayTimeSlider, delayWetLevelSlider, delayFeedbackSlider;
    CustomToggleButton delayBypassButton{"Bypass"};
    DelayPanel::DelayPanel() {
        // Delay labels
        delayTimeSlider.addSliderLabels("0", ((juce::String)MAX_DELAY_TIME), "Time");
        delayWetLevelSlider.addSliderLabels("0", "10", "Wet Mix");
        delayFeedbackSlider.addSliderLabels("0", "10", "Feedback");
        for (auto* comp : getComps()) {
            addAndMakeVisible(comp);
        }
    }

    DelayPanel::~DelayPanel() {}

    std::vector<juce::Component*> getComps() {
        return {
            &delayTimeSlider, &delayWetLevelSlider, &delayFeedbackSlider, &delayBypassButton
        };
    }

    void DelayPanel::resized() override {
        const auto container = getLocalBounds();
        auto delayBounds = container;

        // Title text
        delayBounds.removeFromTop(delayBounds.getHeight() / 4);

        // Add Delay sliders
        auto delayBoundsTopRow = delayBounds.removeFromTop(delayBounds.getHeight() / 2);
        delayTimeSlider.setBounds(delayBoundsTopRow.removeFromLeft(delayBoundsTopRow.getWidth() / 3));
        delayWetLevelSlider.setBounds(delayBoundsTopRow.removeFromLeft(delayBoundsTopRow.getWidth() / 2));
        delayFeedbackSlider.setBounds(delayBoundsTopRow);
        delayBypassButton.setBounds(delayBounds);
    }

    void DelayPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.getHeight() / 4);
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("Delay", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayPanel);
};

// UI component for the reverb module
class ReverbPanel : public Component {
 public:
    // Reverb sliders
    CustomRotarySlider reverbIntensitySlider, reverbRoomSizeSlider, reverbWetMixSlider, reverbSpreadSlider;
    CustomToggleButton reverbBypassButton{"Bypass"}, reverbShimmerButton{"Shimmer"};
    ReverbPanel::ReverbPanel() {
        // Reverb labels
        reverbIntensitySlider.addSliderLabels("0", "10", "Intensity");
        reverbRoomSizeSlider.addSliderLabels("0", "10", "Room Size");
        reverbWetMixSlider.addSliderLabels("0", "10", "Wet Mix");
        reverbSpreadSlider.addSliderLabels("0", "10", "Spread");
        for (auto* comp : getComps()) {
            addAndMakeVisible(comp);
        }
    }

    ReverbPanel::~ReverbPanel() {}

    std::vector<juce::Component*> getComps() {
        return {
            &reverbIntensitySlider, &reverbRoomSizeSlider, &reverbWetMixSlider, &reverbSpreadSlider,
            &reverbBypassButton, &reverbShimmerButton
        };
    }

    void ReverbPanel::resized() override {
        const auto container = getLocalBounds();
        auto reverbBounds = container;

        // Title text
        reverbBounds.removeFromTop(reverbBounds.getHeight() / 4);

        // Add reverb sliders
        auto reverbBoundsTopRow = reverbBounds.removeFromTop(reverbBounds.getHeight() / 3);
        reverbIntensitySlider.setBounds(reverbBoundsTopRow.removeFromLeft(reverbBoundsTopRow.getWidth() / 2));
        reverbWetMixSlider.setBounds(reverbBoundsTopRow);
        auto reverbBoundsMidRow = reverbBounds.removeFromTop(reverbBounds.getHeight() / 2);
        reverbRoomSizeSlider.setBounds(reverbBoundsMidRow.removeFromLeft(reverbBoundsMidRow.getWidth() / 2));
        reverbSpreadSlider.setBounds(reverbBoundsMidRow);
        reverbShimmerButton.setBounds(reverbBounds.removeFromTop(reverbBounds.getHeight() / 2));
        reverbBypassButton.setBounds(reverbBounds);
    }

    void ReverbPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.getHeight() / 4);
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("Reverb", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbPanel);
};

#endif  // USERINTERFACE_MODULEPANELS_H_
