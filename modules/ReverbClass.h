#ifndef MODULES_REVERBCLASS_H_
#define MODULES_REVERBCLASS_H_

//==============================================================================
template <typename Type>
class ReverbUnit {
 public:
    //==============================================================================
    ReverbUnit()
    {}

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec) {
        reverb.prepare(spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept {
        reverb.process(context);
    }

    //==============================================================================
    void reset() noexcept {
        reverb.reset();
    }

    //==============================================================================
    void setParams(ChainSettings chainSettings) {
        // Set reverb parameters

        juce::Reverb::Parameters newParams;

        newParams.damping = 1.f - chainSettings.reverbIntensity;
        newParams.dryLevel = 1.f - chainSettings.reverbWetMix;
        newParams.wetLevel = chainSettings.reverbWetMix;
        newParams.roomSize = chainSettings.reverbRoomSize;
        newParams.width = chainSettings.reverbSpread;

        newParams.freezeMode = chainSettings.reverbShimmer ? 1.f : 0.f;

        reverb.setParameters(newParams);
    }

 private:
    //==============================================================================
    juce::dsp::Reverb reverb;
};

#endif  // MODULES_REVERBCLASS_H_
