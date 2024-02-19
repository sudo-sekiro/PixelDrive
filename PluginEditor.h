#pragma once

#include <vector>

#include "PluginProcessor.h"
#include "UserInterface/CustomSlider.h"
#include "UserInterface/CustomToggle.h"

#include "UserInterface/PresetPanel.h"
#include "UserInterface/ModulePanels.h"

//==============================================================================
class PixelDriveAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        juce::AudioProcessorParameter::Listener,
                                        juce::Timer {
 public:
    explicit PixelDriveAudioProcessorEditor(PixelDriveAudioProcessor&);
    ~PixelDriveAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {
        juce::ignoreUnused(parameterIndex, gestureIsStarting);
    };
    void timerCallback() override;
    void addLabels();

 private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PixelDriveAudioProcessor& processorRef;

    juce::Atomic<bool> parametersChanged { false };

    CustomRotarySlider preGainSlider;

    // Noise gate slider
    CustomRotarySlider noiseGateSlider, outputGainSlider;

    std::vector<juce::Component*> getComps();

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    using ButtonAttachment = APVTS::ButtonAttachment;

    Attachment preGainSliderAttachment,
               distortionPreGainSliderAttachment, distortionToneSliderAttachment, distortionPostGainSliderAttachment,
               distortionClaritySliderAttachment,
               ampInputGainSliderAttachment, ampLowEndSliderAttachment, ampMidsSliderAttachment,
               ampHighEndSliderAttachment,
               delayTimeSliderAttachment, delayWetLevelSliderAttachment, delayFeedbackSliderAttachment,
               reverbIntensitySliderAttachment, reverbRoomSizeSliderAttachment, reverbWetMixSliderAttachment,
               reverbSpreadSliderAttachment,
               noiseGateSliderAttachment,
               outputGainSliderAttachment;

    ButtonAttachment distortionBypassButtonAttachment, ampBypassButtonAttachment, delayBypassButtonAttachment,
                     reverbBypassButtonAttachment, reverbShimmerButtonAttachment;

    UserInterface::PresetPanel presetPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PixelDriveAudioProcessorEditor)
};
