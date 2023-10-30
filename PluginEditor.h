#pragma once

#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }
};

struct CustomToggleButton : juce::ToggleButton { };
//==============================================================================
class PixelDriveAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit PixelDriveAudioProcessorEditor (PixelDriveAudioProcessor&);
    ~PixelDriveAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PixelDriveAudioProcessor& processorRef;

    CustomRotarySlider preGainSlider;

    // Distortion slider
    CustomRotarySlider distortionPreGainSlider, distortionToneSlider, distortionPostGainSlider, distortionClaritySlider;
    CustomToggleButton distortionBypassButton;

    // Amp sliders
    CustomRotarySlider ampInputGainSlider, ampLowEndSlider, ampMidsSlider, ampHighEndSlider;
    CustomToggleButton ampBypassButton;

    // Delay sliders
    CustomRotarySlider delayTimeSlider, delayWetLevelSlider, delayFeedbackSlider;
    CustomToggleButton delayBypassButton;

    // Reverb sliders
    CustomRotarySlider reverbIntensitySlider, reverbRoomSizeSlider, reverbWetMixSlider, reverbSpreadSlider;
    CustomToggleButton reverbBypassButton, reverbShimmerButton;

    // Noise gate slider
    CustomRotarySlider noiseGateSlider;

    std::vector<juce::Component*> getComps();

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    using ButtonAttachment = APVTS::ButtonAttachment;

    Attachment preGainSliderAttachment,
               distortionPreGainSliderAttachment, distortionToneSliderAttachment, distortionPostGainSliderAttachment, distortionClaritySliderAttachment,
               ampInputGainSliderAttachment, ampLowEndSliderAttachment, ampMidsSliderAttachment, ampHighEndSliderAttachment,
               delayTimeSliderAttachment, delayWetLevelSliderAttachment, delayFeedbackSliderAttachment,
               reverbIntensitySliderAttachment, reverbRoomSizeSliderAttachment, reverbWetMixSliderAttachment, reverbSpreadSliderAttachment,
               noiseGateSliderAttachment;

    ButtonAttachment distortionBypassButtonAttachment, ampBypassButtonAttachment, delayBypassButtonAttachment, reverbBypassButtonAttachment, reverbShimmerButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PixelDriveAudioProcessorEditor)
};
