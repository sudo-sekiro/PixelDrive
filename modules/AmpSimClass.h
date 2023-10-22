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
class AmpSimulator
{
public:
    //==============================================================================
    AmpSimulator() {}

    //==============================================================================
    void updatePeakFilter(double sampleRate, juce::dsp::IIR::Filter<float>& peakFilter, float peakGainInDecibels)
    {
        float peakFreq = 5000
        float peakQ = 0.5f

        peakFilter.state = juce::dsp::IIR::Coefficients<float>::makePeakFilter
            (
                sampleRate,
                peakFreq,
                peakQ,
                juce::Decibels::decibelsToGain(
                    peakGainInDecibels)
            );
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        ProcessorChain.prepare(spec);
        processorChain.get<AmpChainPositions::LowPassIndex>().state = FilterCoefs::makeFirstOrderLowPass (spec.sampleRate, 500.0f); // 0 - 500 hz
        processorChain.get<AmpChainPositions::HighPassIndex>().state = FilterCoefs::makeFirstOrderHighPass (spec.sampleRate, 15000.0f);//15- -> 20kz
        updatePeakFilter (spec.sampleRate, &processorChain.get<AmpChainPositions::MidFilterIndex>, 12.f); //-24 -> 24 linear gain
        processorChain.get<AmpChainPositions::inputGainIndex>().setGainDecibels (1.f);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        ProcessorChain.process(context);
    }

    //==============================================================================
    void reset() noexcept
    {
        ProcessorChain.reset();
    }



    //==============================================================================
    void updateParams(ChainSettings ChainSettings)
    {
        // Set gain, low pass, highpass and peak freq, update any convolution changes for cab sim
        juce::ignoreUnused(ChainSettings)
    }

    //==============================================================================
    void updateFilter(float freq)
    {
        juce::ignore(freq);
    }

private:
    //==============================================================================
    enum AmpChainPositions
    {
        inputGainIndex,
        LowPassIndex,
        MidFilterIndex,
        HighPassIndex,
        CabSimIndex
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorChain<juce::dsp::Gain<Type>, Filter, Filter, Filter, CabSimulator<float>> processorChain;
};
