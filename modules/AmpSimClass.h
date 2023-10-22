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
        processorChain.get<AmpChainPositions::LowPassIndex>().state = FilterCoefs::makeFirstOrderHighPass (spec.sampleRate, 500.0f);
        processorChain.get<AmpChainPositions::HighPassIndex>().state = FilterCoefs::makeFirstOrderLowPass (spec.sampleRate, 15000.0f);
        updatePeakFilter (spec.sampleRate, &processorChain.get<AmpChainPositions::MidFilterIndex>, 12.f);
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
    void updateParams(ChainSettings chainSettings)
    {
        // Set gain, low pass, highpass and peak freq, update any convolution changes for cab sim
        #define INPUTRANGEMIN 0
        #define INPUTRANGEMAX 10
        #define LOWPASSFREQ 500
        #define HIGHPASSBASEFREQ 15000
        #define HIGHPASSMAXFREQ 20000
        #define MAXMIDGAIN 24.f
        #define MINMIDGAIN -24.f
        // Set lowpass cutoff frequency in the range 0 - 500 hz
        float lowCutFreq = juce::jmap( chainSettings.ampLowEnd, INPUTRANGEMIN, INPUTRANGEMAX, 500, 0);
        processorChain.get<AmpChainPositions::LowCutIndex>().state = FilterCoefs::makeFirstOrderHighPass (spec.sampleRate, lowCutFreq);
        // Set highpass cutoff frequency in the range 15000 - 20000 hz
        float highCutFreq =  juce::jmap(chainSettings.ampHighEnd, INPUTRANGEMIN, INPUTRANGEMAX, HIGHPASSBASEFREQ, HIGHPASSMAXFREQ);
        processorChain.get<AmpChainPositions::HighCutIndex>().state = FilterCoefs::makeFirstOrderLowPass (spec.sampleRate, highCutFreq);
        // Set gain of the peak filter between -24 and 24 dbs
        float midGain = juce::jmap(chainSettings.ampMids, INPUTRANGEMIN, INPUTRANGEMAX, MINMIDGAIN, MAXMIDGAIN)
        updatePeakFilter (spec.sampleRate, &processorChain.get<AmpChainPositions::MidFilterIndex>, midGain);
        // Set input Gain
        processorChain.get<AmpChainPositions::inputGainIndex>().setGainDecibels (chainSettings.ampInputGain);
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
        LowCutIndex,
        MidFilterIndex,
        HighCutIndex,
        CabSimIndex
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorChain<juce::dsp::Gain<Type>, Filter, Filter, Filter, CabSimulator<float>> processorChain;
};
