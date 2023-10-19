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
