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

        newParams.freezeMode = chainSettings.reverbShimmer ? 1.f : 0.f;

        reverb.setParameters (newParams);
    }

private:
    //==============================================================================
    juce::dsp::Reverb reverb;
};
