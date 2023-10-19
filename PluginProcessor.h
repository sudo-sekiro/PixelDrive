#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "ChainSettings.h"
#include "DelayClass.h"

#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstring>

//==============================================================================
template <typename Type>
class Reverb
{
public:
    //==============================================================================
    Reverb()
    {}

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        reverb.prepare (spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        reverb.process (context);
    }

    //==============================================================================
    void reset() noexcept
    {
        reverb.reset ();
    }

    //==============================================================================
    void setParams(ChainSettings chainSettings)
    {
        // Set reverb parameters
        //auto tone = chainSettings.distortionTone;

        juce::Reverb::Parameters newParams;

        newParams.damping = 1.f - chainSettings.reverbIntensity;
        newParams.dryLevel = 1.f - chainSettings.reverbWetMix;
        newParams.wetLevel = chainSettings.reverbWetMix;
        newParams.roomSize = chainSettings.reverbRoomSize;
        newParams.width = chainSettings.reverbSpread;

        newParams.freezeMode = chainSettings.reverbShimmer ?  1.f : 0.f;

        reverb.setParameters (newParams);
    }

private:
    //==============================================================================
    juce::dsp::Reverb reverb;
};

//==============================================================================
template <typename Type>
class CabSimulator
{
public:
    //==============================================================================
    CabSimulator()
    {
        auto dir = juce::File::getCurrentWorkingDirectory();

        int numTries = 0;

        while (! dir.getChildFile ("Resources").exists() && numTries++ < 15)
            dir = dir.getParentDirectory();

        auto impulseFile = dir.getChildFile ("Resources").getChildFile ("thrash_amp.wav");// ("guitar_amp.wav");
        assert(impulseFile.existsAsFile());

        convolution.loadImpulseResponse (impulseFile,
                                         juce::dsp::Convolution::Stereo::no,
                                         juce::dsp::Convolution::Trim::no,
                                         1024,
                                         juce::dsp::Convolution::Normalise::yes);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        convolution.prepare(spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        convolution.process(context);
    }

    //==============================================================================
    void reset() noexcept
    {
        convolution.reset();
    }

private:
    //==============================================================================

    juce::dsp::Convolution convolution{juce::dsp::Convolution::Latency{ 10 }};
};

//==============================================================================
template <typename Type>
class Distortion
{
public:
    //==============================================================================
    Distortion()
    {
        auto& waveshaper = processorChain.template get<waveshaperIndex>(); // [5]

        waveshaper.functionToUse = [] (Type x)
                                   {
                                       return std::tanh(x);
                                   };

        auto& preGain = processorChain.template get<preGainIndex>();
        preGain.setGainDecibels (50.0f);

        auto& postGain = processorChain.template get<postGainIndex>();
        postGain.setGainDecibels (0.f);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        auto& filter = processorChain.template get<filterIndex>();
        filter.state = FilterCoefs::makeFirstOrderHighPass (spec.sampleRate, 1000.0f);

        processorChain.prepare (spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        processorChain.process (context);
    }

    //==============================================================================
    void reset() noexcept
    {
        processorChain.reset();
    }

    //==============================================================================
    void setParams(ChainSettings chainSettings, double sampleRate)
    {
        // Set how hard the wave shaper clipping is. High tone values approach a squarewave
        auto& waveshaper = processorChain.template get<waveshaperIndex>();
        auto tone = chainSettings.distortionTone;
        waveshaper.functionToUse = [=] (Type x)
                                   {
                                       return std::tanh(tone * x);
                                   };

        auto& preGain = processorChain.template get<preGainIndex>();
        preGain.setGainDecibels (chainSettings.distortionPreGain);

        auto& postGain = processorChain.template get<postGainIndex>();
        postGain.setGainDecibels (chainSettings.distortionPostGain);

        auto& filter = processorChain.template get<filterIndex>();
        filter.state = FilterCoefs::makeFirstOrderHighPass (sampleRate, chainSettings.distortionClarity);
    }

private:
    //==============================================================================
    enum
    {
        filterIndex,
        preGainIndex,
        waveshaperIndex,
        postGainIndex
    };

    using Filter = juce::dsp::IIR::Filter<Type>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<Type>;

    juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::Gain<Type>, juce::dsp::WaveShaper<Type, std::function<Type(Type)>>, juce::dsp::Gain<Type>> processorChain; // [1]
};

//==============================================================================
class PixelDriveAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PixelDriveAudioProcessor();
    ~PixelDriveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr,
                                              "parameters",
                                              createParameterLayout()};

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PixelDriveAudioProcessor)

    enum ChainPositions
    {
        preGainIndex,
        distortionIndex,
        cabSimIndex,
        reverbIndex,
        noiseGateIndex
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    using MonoChain = juce::dsp::ProcessorChain<juce::dsp::Gain<float>, Distortion<float>, CabSimulator<float>, Reverb<float>,juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>>;

    MonoChain leftChain, rightChain;
};
