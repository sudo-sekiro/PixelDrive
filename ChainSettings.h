// Add parameters
struct ChainSettings
{
    float preGain {0.f};
    float distortionTone {1.f}, distortionPreGain {50.f}, distortionPostGain {0.f}, distortionClarity {1000.f};
    float reverbIntensity {0.5f}, reverbWetMix {0.33f}, reverbRoomSize {0.5f}, reverbSpread {1.f};
    bool reverbShimmer {false};
    float noiseGate {0.f};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);