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
        auto toneValue = chainSettings.distortionTone;
        if (lastToneValue != toneValue) {
            toneChanged = true;
            waveShaperLambda = [=] (Type x)
                                   {
                                       return std::tanh(toneValue * x);
                                   };
            lastToneValue = toneValue;
        }

        auto& preGain = processorChain.template get<preGainIndex>();
        preGain.setGainDecibels (chainSettings.distortionPreGain);

        auto& postGain = processorChain.template get<postGainIndex>();
        postGain.setGainDecibels (chainSettings.distortionPostGain);

        auto& filter = processorChain.template get<filterIndex>();
        filter.state = FilterCoefs::makeFirstOrderHighPass (sampleRate, chainSettings.distortionClarity);
    }

    //==============================================================================
    void updateWaveShaper()
    {
        if (toneChanged) {
            auto& waveshaper = processorChain.template get<waveshaperIndex>();
            waveshaper.functionToUse = waveShaperLambda;
            toneChanged = false;
        }
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

    float lastToneValue = 1;
    bool toneChanged = false;
    std::function<Type(Type)> waveShaperLambda;

    using Filter = juce::dsp::IIR::Filter<Type>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<Type>;

    juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, juce::dsp::Gain<Type>, juce::dsp::WaveShaper<Type, std::function<Type(Type)>>, juce::dsp::Gain<Type>> processorChain; // [1]
};
