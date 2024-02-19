// Add parameters

#ifndef CHAINSETTINGS_H_
#define CHAINSETTINGS_H_
struct ChainSettings {
    float preGain {0.f};
    float distortionTone {1.f}, distortionPreGain {50.f}, distortionPostGain {0.f}, distortionClarity {1000.f};
    bool distortionBypass {false};
    float ampInputGain {1.f}, ampLowEnd {0.f}, ampMids {0.f}, ampHighEnd {20000.f};
    bool ampBypass {false};
    float delayTime {0.f}, delayWetLevel {0.f}, delayFeedback {0.f};
    bool delayBypass {false};
    float reverbIntensity {0.5f}, reverbWetMix {0.33f}, reverbRoomSize {0.5f}, reverbSpread {1.f};
    bool reverbShimmer {false}, reverbBypass {false};
    float noiseGate {0.f}, outputGain {0.f};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

#endif  // CHAINSETTINGS_H_
