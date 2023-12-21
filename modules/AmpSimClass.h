#ifndef MODULES_AMPSIMCLASS_H_
#define MODULES_AMPSIMCLASS_H_

//==============================================================================
template <typename Type>
class CabSimulator {
 public:
    //==============================================================================
    CabSimulator() {
        /* Assume thrash_amp.wav has been added as binary data in CMakeLists.txt with `juce_add_binary_data()`
         * Also assume BinaryData.h has been included */
        const char* ampConvolutionData = BinaryData::thrash_amp_wav;
        const int ampConvolutionDataSize = BinaryData::thrash_amp_wavSize;

        convolution.loadImpulseResponse(ampConvolutionData,
                                        ampConvolutionDataSize,
                                        juce::dsp::Convolution::Stereo::no,
                                        juce::dsp::Convolution::Trim::no,
                                        1024,
                                        juce::dsp::Convolution::Normalise::yes);
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec) {
        convolution.prepare(spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept {
        convolution.process(context);
    }

    //==============================================================================
    void reset() noexcept {
        convolution.reset();
    }

 private:
    //==============================================================================

    juce::dsp::Convolution convolution{juce::dsp::Convolution::Latency{ 10 }};
};

//==============================================================================
template <typename Type>
class AmpSimulator {
 public:
    //==============================================================================
    AmpSimulator() {}

    //==============================================================================
    void updatePeakFilter(double sampleRate,
                          juce::dsp::IIR::Filter<float>::CoefficientsPtr peakFilterCoeffs,
                          float peakGainInDecibels) {
        float peakFreq = 1550;
        float peakQ = 0.1f;

        *peakFilterCoeffs = *juce::dsp::IIR::Coefficients<float>::makePeakFilter
        (
            sampleRate,
            peakFreq,
            peakQ,
            juce::Decibels::decibelsToGain(
                peakGainInDecibels));
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec) {
        ampProcessorChain.prepare(spec);
        auto lowCutCoeffs = ampProcessorChain.get<AmpChainPositions::LowCutIndex>().coefficients;
        *lowCutCoeffs = *FilterCoefs::makeFirstOrderHighPass(spec.sampleRate, 1.0f);

        auto highCutCoeffs = ampProcessorChain.get<AmpChainPositions::HighCutIndex>().coefficients;
        *highCutCoeffs = *FilterCoefs::makeFirstOrderLowPass(spec.sampleRate, 20000.0f);

        updatePeakFilter(spec.sampleRate,
                         ampProcessorChain.get<AmpChainPositions::MidFilterIndex>().coefficients,
                         12.f);

        ampProcessorChain.get<AmpChainPositions::inputGainIndex>().setGainDecibels(1.f);

        ampProcessorChain.setBypassed<AmpChainPositions::LowCutIndex>(false);
        ampProcessorChain.setBypassed<AmpChainPositions::HighCutIndex>(false);
        ampProcessorChain.setBypassed<AmpChainPositions::MidFilterIndex>(false);

        auto& waveShaper = ampProcessorChain.template get<waveShaperIndex>();

        waveShaper.functionToUse = [] (Type x) {
                                       return std::tanh(x);
                                   };
    }
    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept {
        ampProcessorChain.process(context);
    }

    //==============================================================================
    void reset() noexcept {
        ampProcessorChain.reset();
    }



    //==============================================================================
    void setParams(ChainSettings chainSettings, double sampleRate) {
        // Set gain, low pass, highpass and peak freq, update any convolution changes for cab sim
        #define INPUTRANGEMIN 0.f
        #define INPUTRANGEMAX 10.f
        #define LOWCUTFREQMIN 1.f
        #define LOWCUTFREQMAX 1500.f
        #define HIGHCUTBASEFREQ 4000.f
        #define HIGHCUTMAXFREQ 20000.f
        #define MAXMIDGAIN 24.f
        #define MINMIDGAIN -24.f

        // Set lowpass cutoff frequency
        auto lowCutFreq = juce::jmap(chainSettings.ampLowEnd,
                                     INPUTRANGEMIN,
                                     INPUTRANGEMAX,
                                     LOWCUTFREQMAX,
                                     LOWCUTFREQMIN);
        auto lowCutCoeffs = ampProcessorChain.get<AmpChainPositions::LowCutIndex>().coefficients;
        *lowCutCoeffs = *FilterCoefs::makeFirstOrderHighPass(sampleRate, lowCutFreq);

        // Set highpass cutoff frequency
        auto highCutFreq =  juce::jmap(chainSettings.ampHighEnd,
                                       INPUTRANGEMIN,
                                       INPUTRANGEMAX,
                                       HIGHCUTBASEFREQ,
                                       HIGHCUTMAXFREQ);
        auto highCutCoeffs = ampProcessorChain.get<AmpChainPositions::HighCutIndex>().coefficients;
        *highCutCoeffs = *FilterCoefs::makeFirstOrderLowPass(sampleRate, highCutFreq);

        // Set gain of the peak filter between -24 and 24 dbs
        auto midGain = juce::jmap(chainSettings.ampMids, INPUTRANGEMIN, INPUTRANGEMAX, MINMIDGAIN, MAXMIDGAIN);
        updatePeakFilter(sampleRate, ampProcessorChain.get<AmpChainPositions::MidFilterIndex>().coefficients, midGain);

        // Set input Gain
        ampProcessorChain.get<AmpChainPositions::inputGainIndex>().setGainDecibels(chainSettings.ampInputGain);
    }

    //==============================================================================
    void updateFilter(float freq) {
        juce::ignore(freq);
    }

 private:
    //==============================================================================
    enum AmpChainPositions {
        inputGainIndex,
        LowCutIndex,
        MidFilterIndex,
        HighCutIndex,
        waveShaperIndex,
        CabSimIndex
    };

    using Filter = juce::dsp::IIR::Filter<Type>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<Type>;

    juce::dsp::ProcessorChain<juce::dsp::Gain<Type>,
                              Filter,
                              Filter,
                              Filter,
                              juce::dsp::WaveShaper<Type, std::function<Type(Type)>>,
                              CabSimulator<float>> ampProcessorChain;
};

#endif  // MODULES_AMPSIMCLASS_H_
