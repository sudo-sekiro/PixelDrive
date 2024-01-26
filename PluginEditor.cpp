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
    distortionPreGainSliderAttachment(p.apvts, "distortionPreGain", p.getDistortionPanel().distortionToneSlider),
    distortionToneSliderAttachment(p.apvts, "distortionTone", p.getDistortionPanel().distortionToneSlider),
    distortionPostGainSliderAttachment(p.apvts, "distortionPostGain", p.getDistortionPanel().distortionPostGainSlider),
    distortionClaritySliderAttachment(p.apvts, "distortionClarity", p.getDistortionPanel().distortionClaritySlider),
    distortionBypassButtonAttachment(p.apvts, "distortionBypass", p.getDistortionPanel().distortionBypassButton),
    // Amp attachments
    ampInputGainSliderAttachment(p.apvts, "ampInputGain", p.getAmpPanel().ampInputGainSlider),
    ampLowEndSliderAttachment(p.apvts, "ampLowEnd", p.getAmpPanel().ampLowEndSlider),
    ampMidsSliderAttachment(p.apvts, "ampMids", p.getAmpPanel().ampMidsSlider),
    ampHighEndSliderAttachment(p.apvts, "ampHighEnd", p.getAmpPanel().ampHighEndSlider),
    ampBypassButtonAttachment(p.apvts, "ampBypass", p.getAmpPanel().ampBypassButton),
    // Delay attachments
    delayTimeSliderAttachment(p.apvts, "delayTime", p.getDelayPanel().delayTimeSlider),
    delayWetLevelSliderAttachment(p.apvts, "delayWetLevel", p.getDelayPanel().delayWetLevelSlider),
    delayFeedbackSliderAttachment(p.apvts, "delayFeedback", p.getDelayPanel().delayFeedbackSlider),
    delayBypassButtonAttachment(p.apvts, "delayBypass", p.getDelayPanel().delayBypassButton),
    // Reverb attachments
    reverbIntensitySliderAttachment(p.apvts, "reverbIntensity", reverbIntensitySlider),
    reverbRoomSizeSliderAttachment(p.apvts, "reverbRoomSize", reverbRoomSizeSlider),
    reverbWetMixSliderAttachment(p.apvts, "reverbWetMix", reverbWetMixSlider),
    reverbSpreadSliderAttachment(p.apvts, "reverbSpread", reverbSpreadSlider),
    reverbBypassButtonAttachment(p.apvts, "reverbBypass", reverbBypassButton),
    reverbShimmerButtonAttachment(p.apvts, "reverbShimmer", reverbShimmerButton),
    noiseGateSliderAttachment(p.apvts, "noiseGate", noiseGateSlider),
    // Preset panel
    presetPanel(p.getPresetManager()) {
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

    addAndMakeVisible(presetPanel);
    addAndMakeVisible(p.getDistortionPanel());
    addAndMakeVisible(p.getAmpPanel());

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
    preGainSlider.setBounds(topBar.removeFromLeft(topBar.getWidth() * 1 / 4));
    presetPanel.setBounds(topBar.removeFromLeft(topBar.getWidth() * 2 / 3));
    noiseGateSlider.setBounds(topBar);

    bounds.reduce(bounds.getWidth() / 25, bounds.getHeight() / 25);

    auto distortionBounds = bounds.removeFromLeft(bounds.getWidth() / 4);
    distortionBounds.setHeight(distortionBounds.getHeight() * 2 / 3);
    distortionBounds.setY(bounds.getHeight() / 2);
    processorRef.getDistortionPanel().setBounds(distortionBounds);

    // Add amp sliders
    auto ampBounds = bounds.removeFromLeft(bounds.getWidth() * 2 / 3);
    // Add amp padding
    ampBounds.removeFromTop(ampBounds.getHeight() / 2);
    ampBounds.removeFromBottom(ampBounds.getHeight() / 2);

    processorRef.getAmpPanel().setBounds(ampBounds);

    // Reverb and delay padding
    bounds.removeFromTop(bounds.getHeight() / 50);
    bounds.removeFromBottom(bounds.getHeight() / 50);
    auto delayBounds = bounds.removeFromTop(bounds.getHeight() / 3);
    processorRef.getDelayPanel().setBounds(delayBounds);

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
