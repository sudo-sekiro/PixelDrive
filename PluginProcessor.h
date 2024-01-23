#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <BinaryData.h>

#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <memory>

#include "ChainSettings.h"
#include "modules/DelayClass.h"
#include "modules/ReverbClass.h"
#include "modules/AmpSimClass.h"
#include "modules/DistortionClass.h"

#include "Service/PresetManager.h"
#include "UserInterface/ModulePanels.h"

//==============================================================================
class PixelDriveAudioProcessor  : public juce::AudioProcessor {
 public:
    //==============================================================================
    PixelDriveAudioProcessor();
    ~PixelDriveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    void updateParameters();

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    void updateNoiseGate(FilterChain& cutChain, float cutoffFreq, double sampleRate);

    Service::PresetManager& getPresetManager() { return *presetManager; }
    DistortionPanel& getDistortionPanel() { return *distortionPanel; }

 private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PixelDriveAudioProcessor);

    enum ChainPositions {
        preGainIndex,
        distortionIndex,
        ampSimIndex,
        delayIndex,
        reverbIndex,
        noiseGateIndex
    };

    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    using MonoChain = juce::dsp::ProcessorChain<juce::dsp::Gain<float>, Distortion<float>, AmpSimulator<float>,
                                                Delay<float, 1>, ReverbUnit<float>, FilterChain>;

    MonoChain leftChain, rightChain;

    std::unique_ptr<Service::PresetManager> presetManager;
    std::unique_ptr<DistortionPanel> distortionPanel;

};
