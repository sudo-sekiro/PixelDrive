#pragma once

#include "PluginProcessor.h"

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PixelDriveAudioProcessorEditor)
};
