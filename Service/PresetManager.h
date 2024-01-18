#pragma once

#include <JuceHeader.h>

namespace Service {
// Handle saving and loading presets to and from text files
class PresetManager : juce::ValueTree::Listener {
 public:
    static const juce::File defaultDirectory;
    static const juce::String extension;
    static const juce::String presetNameProperty;

    explicit PresetManager(juce::AudioProcessorValueTreeState&);
    ~PresetManager();

    void savePreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    int loadNextPreset();
    int loadPreviousPreset();
    juce::StringArray getAllPresets() const;
    juce::String getCurrentPreset() const;

 private:
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::Value currentPreset;
};
}   // namespace Service
