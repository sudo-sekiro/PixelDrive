#pragma once

#ifndef USERINTERFACE_MODULEPANELS_H_
#define USERINTERFACE_MODULEPANELS_H_

#include <JuceHeader.h>

#include "CustomSlider.h"
#include "CustomToggle.h"

#include <memory>
#include <vector>

#define HEADER_SIZE 30
#define MODULE_PADDING 5
#define HEADER_PROPORTION 0.25

#define DISTORTION_ROW_PROPORTION 0.5
#define DISTORTION_TOP_COMPONENT_PROPORTION 0.3333
#define DISTORTION_BOTTOM_COMPONENT_PROPORTION 0.5

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
        const auto container = getLocalBounds().reduced(MODULE_PADDING);
        auto distortionBounds = container;
        auto title = distortionBounds.removeFromTop(distortionBounds.proportionOfHeight(HEADER_PROPORTION));

        // Distortion top row
        auto distortionBoundsTop = distortionBounds.removeFromTop(
            distortionBounds.proportionOfHeight(DISTORTION_ROW_PROPORTION));

        distortionPreGainSlider.setBounds(distortionBoundsTop.removeFromLeft(
            container.proportionOfWidth(DISTORTION_BOTTOM_COMPONENT_PROPORTION)));
        distortionPostGainSlider.setBounds(distortionBoundsTop);

        // Distortion bottom row
        distortionToneSlider.setBounds(distortionBounds.removeFromLeft(
            container.proportionOfWidth(DISTORTION_TOP_COMPONENT_PROPORTION)));
        distortionBypassButton.setBounds(distortionBounds.removeFromLeft(
            container.proportionOfWidth(DISTORTION_TOP_COMPONENT_PROPORTION)));
        distortionClaritySlider.setBounds(distortionBounds);
    }

    void DistortionPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.proportionOfHeight(HEADER_PROPORTION));
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("Distortion", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionPanel);
};

#define AMP_COMPONENT_BOUNDS_PROPORTION 0.33
#define AMP_COMPONENT_PROPORTION 0.2

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
        const auto container = getLocalBounds().reduced(MODULE_PADDING);
        auto ampBounds = container;

        // Title text
        ampBounds.removeFromTop(ampBounds.proportionOfHeight(HEADER_PROPORTION));

        // Add amp sliders
        auto ampBottomBar = ampBounds.removeFromBottom(container.proportionOfHeight(AMP_COMPONENT_BOUNDS_PROPORTION));
        ampInputGainSlider.setBounds(
            ampBottomBar.removeFromLeft(container.proportionOfWidth(AMP_COMPONENT_PROPORTION)));
        ampLowEndSlider.setBounds(ampBottomBar.removeFromLeft(container.proportionOfWidth(AMP_COMPONENT_PROPORTION)));
        ampMidsSlider.setBounds(ampBottomBar.removeFromLeft(container.proportionOfWidth(AMP_COMPONENT_PROPORTION)));
        ampHighEndSlider.setBounds(ampBottomBar.removeFromLeft(container.proportionOfWidth(AMP_COMPONENT_PROPORTION)));
        ampBypassButton.setBounds(ampBottomBar);
    }

    void AmpPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.proportionOfHeight(HEADER_PROPORTION));
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("Amp", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpPanel);
};

#define DELAY_COMPONENT_PROPORTION 0.25

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
        auto delayBounds = getLocalBounds().reduced(MODULE_PADDING);

        // Title text
        delayBounds.removeFromTop(delayBounds.proportionOfHeight(HEADER_PROPORTION));
        const auto container = delayBounds;

        // Add Delay sliders
        delayTimeSlider.setBounds(
            delayBounds.removeFromTop(container.proportionOfHeight(DELAY_COMPONENT_PROPORTION)));
        delayWetLevelSlider.setBounds(
            delayBounds.removeFromTop(container.proportionOfHeight(DELAY_COMPONENT_PROPORTION)));
        delayFeedbackSlider.setBounds(
            delayBounds.removeFromTop(container.proportionOfHeight(DELAY_COMPONENT_PROPORTION)));
        delayBypassButton.setBounds(delayBounds);
    }

    void DelayPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.proportionOfHeight(HEADER_PROPORTION));
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("Delay", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayPanel);
};

#define REVERB_ROW_PROPORTION 0.3333
#define REVERB_BUTTON_ROW_PROPORTION 0.5
#define REVERB_COMPONENT_PROPORTION 0.5

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
        auto reverbBounds = getLocalBounds().reduced(MODULE_PADDING);

        // Title text
        reverbBounds.removeFromTop(reverbBounds.proportionOfHeight(HEADER_PROPORTION));
        const auto container = reverbBounds;

        // Add reverb sliders
        auto reverbBoundsTopRow = reverbBounds.removeFromTop(container.proportionOfHeight(REVERB_ROW_PROPORTION));
        reverbIntensitySlider.setBounds(
            reverbBoundsTopRow.removeFromLeft(container.proportionOfWidth(REVERB_COMPONENT_PROPORTION)));
        reverbWetMixSlider.setBounds(reverbBoundsTopRow);

        auto reverbBoundsMidRow = reverbBounds.removeFromTop(container.proportionOfHeight(REVERB_ROW_PROPORTION));
        reverbRoomSizeSlider.setBounds(
            reverbBoundsMidRow.removeFromLeft(container.proportionOfWidth(REVERB_COMPONENT_PROPORTION)));
        reverbSpreadSlider.setBounds(reverbBoundsMidRow);

        reverbShimmerButton.setBounds(
            reverbBounds.removeFromTop(reverbBounds.proportionOfHeight(REVERB_BUTTON_ROW_PROPORTION)));
        reverbBypassButton.setBounds(reverbBounds);
    }

    void ReverbPanel::paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds();
        g.drawRect(bounds);
        auto titleText = bounds.removeFromTop(bounds.proportionOfHeight(HEADER_PROPORTION));
        g.setColour(Colour(0u, 172u, 1u));
        g.setFont(static_cast<float>(HEADER_SIZE));
        g.drawFittedText("Reverb", titleText.toNearestInt(), juce::Justification::centred, 1);
    }

 private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbPanel);
};

#endif  // USERINTERFACE_MODULEPANELS_H_
