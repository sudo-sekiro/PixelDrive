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

        ampProcessorChain.get<AmpChainPositions::inputGainIndex>().setGainDecibels(1.f);

        ampProcessorChain.setBypassed<AmpChainPositions::lowCutIndex>(false);
        ampProcessorChain.setBypassed<AmpChainPositions::midFilterIndex>(false);
        ampProcessorChain.setBypassed<AmpChainPositions::highShelfIndex>(false);
        ampProcessorChain.setBypassed<AmpChainPositions::lowShelfIndex>(false);

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
    // Set gain, low cut, high shelf, low shelf and peak filter parameters
    void setParams(ChainSettings chainSettings, double sampleRate) {
        #define INPUT_RANGE_MIN 0.f
        #define INPUT_RANGE_MAX 10.f

        #define LOWCUT_FREQ_MIN 1.f
        #define LOWCUT_FREQ_MAX 65.f

        #define MID_GAIN_MAX -9.f
        #define MID_GAIN_MIN -20.f

        #define SHELF_FILTER_CUTOFF_FREQUENCY 1000.f
        #define SHELF_FILTER_Q_VALUE 0.7f

        #define HIGH_SHELF_GAIN_FACTOR_MIN 0.1f
        #define HIGH_SHELF_GAIN_FACTOR_MAX 1.f

        #define LOW_SHELF_GAIN_DENOMINATOR_MIN 1.f
        #define LOW_SHELF_GAIN_DENOMINATOR_MAX 10.f
        #define LOW_SHELF_GAIN_FACTOR_BASE 0.9f
        #define LOW_SHELF_GAIN_NUMERATOR_MIN 0.9f
        #define LOW_SHELF_GAIN_NUMERATOR_MAX 1.f

        /* Set lowpass cutoff frequency.
         * This will increase as the bass input decreases to add a slope to the low end response as bass is decreased. */
        auto lowCutFreq = juce::jmap(chainSettings.ampLowEnd,
                                     INPUT_RANGE_MIN,
                                     INPUT_RANGE_MAX,
                                     LOWCUT_FREQ_MAX,
                                     LOWCUT_FREQ_MIN);
        auto lowCutCoeffs = ampProcessorChain.get<AmpChainPositions::lowCutIndex>().coefficients;
        *lowCutCoeffs = *FilterCoefs::makeFirstOrderHighPass(sampleRate, lowCutFreq);

        /* Set high shelf gain.
         * This value is mapped from 0.1 to 1 so that the gain of the high shelf filter varies with the treble input.
         * When the gain factor is 1 there is 0 dB attenuatiion and when it is 0.1 there is 10 dB attenuation on the
         * high end frequency band */
        auto highShelfGain =  juce::jmap(chainSettings.ampHighEnd,
                                       INPUT_RANGE_MIN,
                                       INPUT_RANGE_MAX,
                                       HIGH_SHELF_GAIN_FACTOR_MIN,
                                       HIGH_SHELF_GAIN_FACTOR_MAX);
        auto highShelfCoeffs = ampProcessorChain.get<AmpChainPositions::highShelfIndex>().coefficients;
        *highShelfCoeffs = *FilterCoefs::makeHighShelf(sampleRate,
                                                       SHELF_FILTER_CUTOFF_FREQUENCY,
                                                       SHELF_FILTER_Q_VALUE,
                                                       highShelfGain);
        /* Divide low shelf gain proportional to the bass input.
         * This attenuates the mid and low range frequncy bands as the bass input is lowered. */
        auto lowShelfGainDenominator = juce::jmap(chainSettings.ampLowEnd,
                                                  INPUT_RANGE_MIN,
                                                  INPUT_RANGE_MAX,
                                                  LOW_SHELF_GAIN_DENOMINATOR_MAX,
                                                  LOW_SHELF_GAIN_DENOMINATOR_MIN);

        /* When the trebble knob is high, attenuate the low end.
         * This will mimic hardware tone stacks where the low end is boosted when the high end potentiomter is lowered. */
        auto lowShelfGainNumerator = juce::jmap(chainSettings.ampHighEnd,
                                                INPUT_RANGE_MIN,
                                                INPUT_RANGE_MAX,
                                                LOW_SHELF_GAIN_NUMERATOR_MAX,
                                                LOW_SHELF_GAIN_NUMERATOR_MIN);

        auto lowShelfCoeffs = ampProcessorChain.get<AmpChainPositions::lowShelfIndex>().coefficients;
        *lowShelfCoeffs = *FilterCoefs::makeLowShelf(sampleRate,
                                                     SHELF_FILTER_CUTOFF_FREQUENCY,
                                                     SHELF_FILTER_Q_VALUE,
                                                     lowShelfGainNumerator / lowShelfGainDenominator);

        /* Set gain of the peak filter between -9 and -20 dbs.
         * Guitar pickups naturally boost the mid frequencies so the midband should always be attenuated to balance the
         * frequencies. */
        auto midGain = juce::jmap(chainSettings.ampMids, INPUT_RANGE_MIN, INPUT_RANGE_MAX, MID_GAIN_MIN, MID_GAIN_MAX);
        updatePeakFilter(sampleRate, ampProcessorChain.get<AmpChainPositions::midFilterIndex>().coefficients, midGain);

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
        lowCutIndex,
        midFilterIndex,
        highShelfIndex,
        lowShelfIndex,
        waveShaperIndex,
        cabSimIndex
    };

    using Filter = juce::dsp::IIR::Filter<Type>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<Type>;

    juce::dsp::ProcessorChain<juce::dsp::Gain<Type>,
                              Filter,
                              Filter,
                              Filter,
                              Filter,
                              juce::dsp::WaveShaper<Type, std::function<Type(Type)>>,
                              CabSimulator<float>> ampProcessorChain;
};

#endif  // MODULES_AMPSIMCLASS_H_
