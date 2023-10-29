#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PixelDriveAudioProcessorEditor::PixelDriveAudioProcessorEditor (PixelDriveAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }
    setSize (900, 600);
}

PixelDriveAudioProcessorEditor::~PixelDriveAudioProcessorEditor()
{
}

//==============================================================================
void PixelDriveAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PixelDriveAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto topBar = bounds.removeFromTop(bounds.getHeight() / 6);
    preGainSlider.setBounds(topBar.removeFromLeft(topBar.getWidth() / 2));
    noiseGateSlider.setBounds(topBar);

    bounds.reduce(bounds.getWidth() / 25, bounds.getHeight() / 25);

    auto distortionBounds = bounds.removeFromLeft(bounds.getWidth() / 4);

    // Distortion top row
    auto distortionBoundsTop = distortionBounds.removeFromTop(distortionBounds.getHeight() / 2);
    distortionToneSlider.setBounds(distortionBoundsTop.removeFromLeft(distortionBoundsTop.getWidth() / 3));
    distortionBypassButton.setBounds(distortionBoundsTop.removeFromLeft(distortionBoundsTop.getWidth() / 2));
    distortionClaritySlider.setBounds(distortionBoundsTop);
    // Distortion bottom row
    distortionPostGainSlider.setBounds(distortionBounds.removeFromLeft(distortionBounds.getWidth() / 2));
    distortionPreGainSlider.setBounds(distortionBounds);
    // Add amp sliders
    auto ampBounds = bounds.removeFromLeft(bounds.getWidth() * 2 / 3);
    // Add amp padding
    ampBounds.removeFromTop(ampBounds.getHeight() / 10);
    ampBounds.removeFromBottom(ampBounds.getHeight() / 10);
    // Add amp sliders
    ampInputGainSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 5));
    ampLowEndSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 4));
    ampMidsSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 3));
    ampHighEndSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 2));
    ampBypassButton.setBounds(ampBounds);
    // Reverb and delay padding
    bounds.removeFromTop(bounds.getHeight() / 10);
    bounds.removeFromBottom(bounds.getHeight() / 10);
    auto delayBounds = bounds.removeFromTop(bounds.getHeight() / 2);
    auto delayBoundsTopRow = delayBounds.removeFromTop(delayBounds.getHeight() / 2);
    delayTimeSlider.setBounds(delayBoundsTopRow.removeFromLeft(delayBoundsTopRow.getWidth() / 3));
    delayWetLevelSlider.setBounds(delayBoundsTopRow.removeFromLeft(delayBoundsTopRow.getWidth() / 2));
    delayFeedbackSlider.setBounds(delayBoundsTopRow);
    delayBypassButton.setBounds(delayBounds);
    //Add reverb sliders
    auto reverbBoundsTopRow = bounds.removeFromTop(bounds.getHeight() / 3);
    reverbIntensitySlider.setBounds(reverbBoundsTopRow.removeFromLeft(reverbBoundsTopRow.getWidth() / 2));
    reverbWetMixSlider.setBounds(reverbBoundsTopRow);
    auto reverbBoundsMidRow = bounds.removeFromTop(bounds.getHeight() / 2);
    reverbRoomSizeSlider.setBounds(reverbBoundsMidRow.removeFromLeft(reverbBoundsMidRow.getWidth() / 2));
    reverbSpreadSlider.setBounds(reverbBoundsMidRow);
    reverbShimmerButton.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
    reverbBypassButton.setBounds(bounds);
}

std::vector<juce::Component*> PixelDriveAudioProcessorEditor::getComps()
{
    return
    {
        &preGainSlider,
        &distortionPreGainSlider, &distortionToneSlider, &distortionPostGainSlider, &distortionClaritySlider, &distortionBypassButton,
        &ampInputGainSlider, &ampLowEndSlider, &ampMidsSlider, &ampHighEndSlider, &ampBypassButton,
        &delayTimeSlider, &delayWetLevelSlider, &delayFeedbackSlider, &delayBypassButton,
        &reverbIntensitySlider, &reverbShimmerButton, &reverbRoomSizeSlider, &reverbWetMixSlider, &reverbSpreadSlider, &reverbBypassButton,
        &noiseGateSlider
    };
}
