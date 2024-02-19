#include <vector>

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UserInterface/CustomSlider.cpp"
#include "UserInterface/CustomToggle.cpp"

#define MODULE_PADDING 5
#define BACKGROUND_COLOUR 0xfffcffb8

//==============================================================================
PixelDriveAudioProcessorEditor::PixelDriveAudioProcessorEditor(PixelDriveAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p),
    preGainSliderAttachment(p.apvts, "preGain", preGainSlider),
    // Distortion attachments
    distortionPreGainSliderAttachment(p.apvts, "distortionPreGain", p.getDistortionPanel().distortionPreGainSlider),
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
    reverbIntensitySliderAttachment(p.apvts, "reverbIntensity", p.getReverbPanel().reverbIntensitySlider),
    reverbRoomSizeSliderAttachment(p.apvts, "reverbRoomSize", p.getReverbPanel().reverbRoomSizeSlider),
    reverbWetMixSliderAttachment(p.apvts, "reverbWetMix", p.getReverbPanel().reverbWetMixSlider),
    reverbSpreadSliderAttachment(p.apvts, "reverbSpread", p.getReverbPanel().reverbSpreadSlider),
    reverbBypassButtonAttachment(p.apvts, "reverbBypass", p.getReverbPanel().reverbBypassButton),
    reverbShimmerButtonAttachment(p.apvts, "reverbShimmer", p.getReverbPanel().reverbShimmerButton),
    // Noise gate attachment
    noiseGateSliderAttachment(p.apvts, "noiseGate", noiseGateSlider),
    outputGainSliderAttachment(p.apvts, "outputGain", outputGainSlider),
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
    addAndMakeVisible(p.getDelayPanel());
    addAndMakeVisible(p.getReverbPanel());

    startTimerHz(60);

    setSize(1080, 600);
}

PixelDriveAudioProcessorEditor::~PixelDriveAudioProcessorEditor() {
    const auto& params = processorRef.getParameters();
    for ( auto param : params ) {
        param->removeListener(this);
    }
}

//==============================================================================
void PixelDriveAudioProcessorEditor::paint(juce::Graphics& g) {
    const auto container = getLocalBounds();
    auto bounds = container;
    // Fill the window with our background colour
    g.setColour(juce::Colour(BACKGROUND_COLOUR));
    g.fillRect(bounds);
}

#define BOUNDS_WIDTH_PADDING 0.02
#define PEDAL_HEIGHT_PADDING_PROPORTION 0.125
#define AMP_HEIGHT_PADDING_PROPORTION 0.1

#define TOP_BAR_HEIGHT_PROPORTION 0.15
#define TOP_BAR_PREGAIN_PROPORTION 0.25
#define TOP_BAR_PRESET_PROPORTION 0.5
#define TOP_BAR_NOISEGATE_PROPORTION 0.125

#define DISTORTION_WIDTH_PROPORTION 0.2
#define AMP_WIDTH_PROPORTION 0.4
#define DELAY_WIDTH_PROPORTION 0.15

void PixelDriveAudioProcessorEditor::resized() {
    const auto container = getLocalBounds();
    auto bounds = container;

    // Add sliders and preset manager to the topbar
    auto topBar = bounds.removeFromTop(container.proportionOfHeight(TOP_BAR_HEIGHT_PROPORTION));
    preGainSlider.setBounds(topBar.removeFromLeft(container.proportionOfWidth(TOP_BAR_PREGAIN_PROPORTION)));
    presetPanel.setBounds(topBar.removeFromLeft(container.proportionOfWidth(TOP_BAR_PRESET_PROPORTION)));
    noiseGateSlider.setBounds(topBar.removeFromLeft(container.proportionOfWidth(TOP_BAR_NOISEGATE_PROPORTION)));
    outputGainSlider.setBounds(topBar);

    // Add horizontal padding to the region below the top bar
    bounds.reduce(container.proportionOfWidth(BOUNDS_WIDTH_PADDING), 0);

    auto distortionBounds = bounds.removeFromLeft(container.proportionOfWidth(DISTORTION_WIDTH_PROPORTION));
    // Add vertical padding to distortion module
    distortionBounds.reduce(0, container.proportionOfHeight(PEDAL_HEIGHT_PADDING_PROPORTION));
    // Set distortion bounds
    processorRef.getDistortionPanel().setBounds(distortionBounds.reduced(MODULE_PADDING));

    auto ampBounds = bounds.removeFromLeft(container.proportionOfWidth(AMP_WIDTH_PROPORTION));
    // Add vertical padding to amp module
    ampBounds.reduce(0, container.proportionOfHeight(AMP_HEIGHT_PADDING_PROPORTION));
    // Set amp module bounds
    processorRef.getAmpPanel().setBounds(ampBounds.reduced(MODULE_PADDING));

    // Add vertical padding to the reverb and delay modules
    bounds.reduce(0, container.proportionOfHeight(PEDAL_HEIGHT_PADDING_PROPORTION));
    // Set delay module bounds
    auto delayBounds = bounds.removeFromRight(container.proportionOfWidth(DELAY_WIDTH_PROPORTION));
    processorRef.getDelayPanel().setBounds(delayBounds.reduced(MODULE_PADDING));

    // Set reverb module bounds
    processorRef.getReverbPanel().setBounds(bounds.reduced(MODULE_PADDING));
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
        &noiseGateSlider,
        &outputGainSlider
    };
}

void PixelDriveAudioProcessorEditor::addLabels() {
    /* Add label, max and min values */
    // Pregain
    preGainSlider.addSliderLabels("-24dB", "24dB", "Pregain");

    // Noise Gate
    noiseGateSlider.addSliderLabels("100", "20000", "Noise Gate");
    // Output Gain
    outputGainSlider.addSliderLabels("-24dB", "24dB", "Output Gain");
}
