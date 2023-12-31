#include <vector>

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UserInterface/CustomSlider.cpp"
#include "UserInterface/CustomToggle.cpp"

//==============================================================================
PixelDriveAudioProcessorEditor::PixelDriveAudioProcessorEditor(PixelDriveAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p),
    preGainSliderAttachment(p.apvts, "preGain", preGainSlider),
    // Distortion attachments
    distortionPreGainSliderAttachment(p.apvts, "distortionPreGain", distortionPreGainSlider),
    distortionToneSliderAttachment(p.apvts, "distortionTone", distortionToneSlider),
    distortionPostGainSliderAttachment(p.apvts, "distortionPostGain", distortionPostGainSlider),
    distortionClaritySliderAttachment(p.apvts, "distortionClarity", distortionClaritySlider),
    distortionBypassButtonAttachment(p.apvts, "distortionBypass", distortionBypassButton),
    // Amp attachments
    ampInputGainSliderAttachment(p.apvts, "ampInputGain", ampInputGainSlider),
    ampLowEndSliderAttachment(p.apvts, "ampLowEnd", ampLowEndSlider),
    ampMidsSliderAttachment(p.apvts, "ampMids", ampMidsSlider),
    ampHighEndSliderAttachment(p.apvts, "ampHighEnd", ampHighEndSlider),
    ampBypassButtonAttachment(p.apvts, "ampBypass", ampBypassButton),
    // Delay attachments
    delayTimeSliderAttachment(p.apvts, "delayTime", delayTimeSlider),
    delayWetLevelSliderAttachment(p.apvts, "delayWetLevel", delayWetLevelSlider),
    delayFeedbackSliderAttachment(p.apvts, "delayFeedback", delayFeedbackSlider),
    delayBypassButtonAttachment(p.apvts, "delayBypass", delayBypassButton),
    // Reverb attachments
    reverbIntensitySliderAttachment(p.apvts, "reverbIntensity", reverbIntensitySlider),
    reverbRoomSizeSliderAttachment(p.apvts, "reverbRoomSize", reverbRoomSizeSlider),
    reverbWetMixSliderAttachment(p.apvts, "reverbWetMix", reverbWetMixSlider),
    reverbSpreadSliderAttachment(p.apvts, "reverbSpread", reverbSpreadSlider),
    reverbBypassButtonAttachment(p.apvts, "reverbBypass", reverbBypassButton),
    reverbShimmerButtonAttachment(p.apvts, "reverbShimmer", reverbShimmerButton),
    noiseGateSliderAttachment(p.apvts, "noiseGate", noiseGateSlider)
{
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addLabels();

    for (auto* comp : getComps()) {
        addAndMakeVisible(comp);
    }

    const auto& params = processorRef.getParameters();
    for ( auto param : params ) {
        param->addListener(this);
    }

    startTimerHz(60);

    setSize(900, 600);
}

PixelDriveAudioProcessorEditor::~PixelDriveAudioProcessorEditor() {
    const auto& params = processorRef.getParameters();
    for ( auto param : params ) {
        param->removeListener(this);
    }
}

//==============================================================================
void PixelDriveAudioProcessorEditor::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PixelDriveAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();

    auto topBar = bounds.removeFromTop(bounds.getHeight() / 6);
    preGainSlider.setBounds(topBar.removeFromLeft(topBar.getWidth() / 2));
    noiseGateSlider.setBounds(topBar);

    bounds.reduce(bounds.getWidth() / 25, bounds.getHeight() / 25);

    auto distortionBounds = bounds.removeFromLeft(bounds.getWidth() / 4);

    // Distortion top row
    distortionBounds.removeFromTop(distortionBounds.getHeight() / 4);
    auto distortionBoundsTop = distortionBounds.removeFromTop(distortionBounds.getHeight() / 4);
    distortionToneSlider.setBounds(distortionBoundsTop.removeFromLeft(distortionBoundsTop.getWidth() / 3));
    distortionBypassButton.setBounds(distortionBoundsTop.removeFromLeft(distortionBoundsTop.getWidth() / 2));
    distortionClaritySlider.setBounds(distortionBoundsTop);
    // Distortion bottom row
    distortionPostGainSlider.setBounds(distortionBounds.removeFromLeft(distortionBounds.getWidth() / 2));
    distortionPreGainSlider.setBounds(distortionBounds);
    // Add amp sliders
    auto ampBounds = bounds.removeFromLeft(bounds.getWidth() * 2 / 3);
    // Add amp padding
    ampBounds.removeFromTop(ampBounds.getHeight() / 2);
    ampBounds.removeFromBottom(ampBounds.getHeight() / 2);
    // Add amp sliders
    ampInputGainSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 5));
    ampLowEndSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 4));
    ampMidsSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 3));
    ampHighEndSlider.setBounds(ampBounds.removeFromLeft(ampBounds.getWidth() / 2));
    ampBypassButton.setBounds(ampBounds);
    // Reverb and delay padding
    bounds.removeFromTop(bounds.getHeight() / 20);
    bounds.removeFromBottom(bounds.getHeight() / 20);
    auto delayBounds = bounds.removeFromTop(bounds.getHeight() / 3);
    auto delayBoundsTopRow = delayBounds.removeFromTop(delayBounds.getHeight() / 2);
    delayTimeSlider.setBounds(delayBoundsTopRow.removeFromLeft(delayBoundsTopRow.getWidth() / 3));
    delayWetLevelSlider.setBounds(delayBoundsTopRow.removeFromLeft(delayBoundsTopRow.getWidth() / 2));
    delayFeedbackSlider.setBounds(delayBoundsTopRow);
    delayBypassButton.setBounds(delayBounds);
    // Add reverb sliders
    bounds.removeFromBottom(bounds.getHeight() / 20);
    auto reverbBoundsTopRow = bounds.removeFromTop(bounds.getHeight() / 3);
    reverbIntensitySlider.setBounds(reverbBoundsTopRow.removeFromLeft(reverbBoundsTopRow.getWidth() / 2));
    reverbWetMixSlider.setBounds(reverbBoundsTopRow);
    auto reverbBoundsMidRow = bounds.removeFromTop(bounds.getHeight() / 2);
    reverbRoomSizeSlider.setBounds(reverbBoundsMidRow.removeFromLeft(reverbBoundsMidRow.getWidth() / 2));
    reverbSpreadSlider.setBounds(reverbBoundsMidRow);
    reverbShimmerButton.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
    reverbBypassButton.setBounds(bounds);
}

void PixelDriveAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue) {
    juce::ignoreUnused(parameterIndex, newValue);
    parametersChanged.set(true);
}

void PixelDriveAudioProcessorEditor::timerCallback() {
    if (parametersChanged.compareAndSetBool(false, true)) {
        processorRef.updateParameters();
    }
}

std::vector<juce::Component*> PixelDriveAudioProcessorEditor::getComps() {
    return {
        &preGainSlider,
        &distortionPreGainSlider, &distortionToneSlider, &distortionPostGainSlider, &distortionClaritySlider,
        &distortionBypassButton,
        &ampInputGainSlider, &ampLowEndSlider, &ampMidsSlider, &ampHighEndSlider, &ampBypassButton,
        &delayTimeSlider, &delayWetLevelSlider, &delayFeedbackSlider, &delayBypassButton,
        &reverbIntensitySlider, &reverbShimmerButton, &reverbRoomSizeSlider, &reverbWetMixSlider, &reverbSpreadSlider,
        &reverbBypassButton,
        &noiseGateSlider
    };
}

void PixelDriveAudioProcessorEditor::addLabels() {
    /* Add label, max and min values */
    // Pregain
    preGainSlider.addSliderLabels("-24dB", "24dB", "Pregain");

    // Distortion labels
    distortionPreGainSlider.addSliderLabels("-10dB", "100dB", "Input Gain");
    distortionToneSlider.addSliderLabels("0", "10", "Tone");
    distortionPostGainSlider.addSliderLabels("-24dB", "24dB", "Output Gain");
    distortionClaritySlider.addSliderLabels("0", "10", "Clarity");
    // distortionBypassButton

    // Amp labels
    ampInputGainSlider.addSliderLabels("0", "11", "Input Gain");
    ampLowEndSlider.addSliderLabels("0", "11", "Bass");
    ampMidsSlider.addSliderLabels("0", "11", "Mids");
    ampHighEndSlider.addSliderLabels("0", "11", "Treble");

    // Delay labels
    delayTimeSlider.addSliderLabels("0", ((juce::String)MAX_DELAY_TIME), "Time");
    delayWetLevelSlider.addSliderLabels("0", "10", "Wet Mix");
    delayFeedbackSlider.addSliderLabels("0", "10", "Feedback");

    // Reverb labels
    reverbIntensitySlider.addSliderLabels("0", "10", "Intensity");
    reverbRoomSizeSlider.addSliderLabels("0", "10", "Room Size");
    reverbWetMixSlider.addSliderLabels("0", "10", "Wet Mix");
    reverbSpreadSlider.addSliderLabels("0", "10", "Spread");

    // Noise Gate
    noiseGateSlider.addSliderLabels("100", "20000", "Noise Gate");
}
