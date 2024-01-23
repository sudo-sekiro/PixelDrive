#include <memory>

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ClampOutput.h"

//==============================================================================
PixelDriveAudioProcessor::PixelDriveAudioProcessor()
     : AudioProcessor(BusesProperties()
                     #if !JucePlugin_IsMidiEffect
                      #if !JucePlugin_IsSynth
                       .withInput("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                     ),
                    apvts(*this, nullptr, ProjectInfo::projectName, PixelDriveAudioProcessor::createParameterLayout()) {
                        apvts.state.setProperty(Service::PresetManager::presetNameProperty, "", nullptr);
                        apvts.state.setProperty("version", ProjectInfo::versionNumber, nullptr);
                        presetManager = std::make_unique<Service::PresetManager>(apvts);
                        distortionPanel = std::make_unique<DistortionPanel>();
                    }

PixelDriveAudioProcessor::~PixelDriveAudioProcessor() {}

//==============================================================================
const juce::String PixelDriveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PixelDriveAudioProcessor::acceptsMidi() const {
    #if JucePlugin_WantsMidiInput
        return true;
    #else
        return false;
    #endif
}

bool PixelDriveAudioProcessor::producesMidi() const {
    #if JucePlugin_ProducesMidiOutput
        return true;
    #else
        return false;
    #endif
}

bool PixelDriveAudioProcessor::isMidiEffect() const {
    #if JucePlugin_IsMidiEffect
        return true;
    #else
        return false;
    #endif
}

double PixelDriveAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int PixelDriveAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PixelDriveAudioProcessor::getCurrentProgram() {
    return 0;
}

void PixelDriveAudioProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String PixelDriveAudioProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void PixelDriveAudioProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void PixelDriveAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;

    spec.sampleRate = sampleRate;

    auto& leftNoiseGate = leftChain.get<ChainPositions::noiseGateIndex>();
    updateNoiseGate(leftNoiseGate, 17500, sampleRate);

    auto& rightNoiseGate = rightChain.get<ChainPositions::noiseGateIndex>();
    updateNoiseGate(rightNoiseGate, 17500, sampleRate);

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    // Set initial plugin gain
    auto& leftPreGain = leftChain.template get<ChainPositions::preGainIndex>();
    auto& rightPreGain = rightChain.template get<ChainPositions::preGainIndex>();

    auto chainSettings = getChainSettings(apvts);

    leftPreGain.setGainDecibels(chainSettings.preGain);
    rightPreGain.setGainDecibels(chainSettings.preGain);

    updateParameters();
}

void PixelDriveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PixelDriveAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    #if JucePlugin_IsMidiEffect
        juce::ignoreUnused(layouts);
        return true;
    #else
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        // Some plugin hosts, such as certain GarageBand versions, will only
        // load plugins that support stereo bus layouts.
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        // This checks if the input layout matches the output layout
        #if !JucePlugin_IsSynth
            if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
                return false;
        #endif

        return true;
    #endif
}

void PixelDriveAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    juce::dsp::AudioBlock<float> block(buffer);

    auto numSamples = block.getNumSamples();
    // Clamp output to prevent feedback
    protectYourEars(buffer, numSamples, totalNumInputChannels);

    auto LeftBlock = block.getSingleChannelBlock(0);
    auto RightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(LeftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(RightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    auto& leftDistortion = leftChain.template get<ChainPositions::distortionIndex>();
    auto& rightDistortion = rightChain.template get<ChainPositions::distortionIndex>();
    leftDistortion.updateWaveShaper();
    rightDistortion.updateWaveShaper();
}

//==============================================================================
bool PixelDriveAudioProcessor::hasEditor() const {
    return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PixelDriveAudioProcessor::createEditor() {
    return new PixelDriveAudioProcessorEditor (*this);
    // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void PixelDriveAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void PixelDriveAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

// Add parameter
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts) {
    ChainSettings settings;

    settings.preGain = apvts.getRawParameterValue("preGain")->load();

    // Return distortion parameters
    settings.distortionPreGain = apvts.getRawParameterValue("distortionPreGain")->load();
    settings.distortionTone = apvts.getRawParameterValue("distortionTone")->load();
    settings.distortionPostGain = apvts.getRawParameterValue("distortionPostGain")->load();
    settings.distortionClarity = apvts.getRawParameterValue("distortionClarity")->load();
    settings.distortionBypass = apvts.getRawParameterValue("distortionBypass")->load();

    // Return amp settings
    settings.ampInputGain = apvts.getRawParameterValue("ampInputGain")->load();
    settings.ampLowEnd = apvts.getRawParameterValue("ampLowEnd")->load();
    settings.ampMids = apvts.getRawParameterValue("ampMids")->load();
    settings.ampHighEnd = apvts.getRawParameterValue("ampHighEnd")->load();
    settings.ampBypass = apvts.getRawParameterValue("ampBypass")->load();

    // Return delay parameters
    settings.delayTime = apvts.getRawParameterValue("delayTime")->load();
    settings.delayWetLevel = apvts.getRawParameterValue("delayWetLevel")->load();
    settings.delayFeedback = apvts.getRawParameterValue("delayFeedback")->load();
    settings.delayBypass = apvts.getRawParameterValue("delayBypass")->load();

    // Return reverb parameters
    settings.reverbIntensity = apvts.getRawParameterValue("reverbIntensity")->load();
    settings.reverbShimmer = apvts.getRawParameterValue("reverbShimmer")->load();
    settings.reverbRoomSize = apvts.getRawParameterValue("reverbRoomSize")->load();
    settings.reverbWetMix = apvts.getRawParameterValue("reverbWetMix")->load();
    settings.reverbSpread = apvts.getRawParameterValue("reverbSpread")->load();
    settings.reverbBypass = apvts.getRawParameterValue("reverbBypass")->load();

    settings.noiseGate = apvts.getRawParameterValue("noiseGate")->load();

    return settings;
}

// Add parameters
juce::AudioProcessorValueTreeState::ParameterLayout
    PixelDriveAudioProcessor::createParameterLayout() {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterFloat>("preGain", "preGain",
                                    juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                    0.0f));
        /* Distortion parameters
         * distortionPreGain: Input gain to the distortion class
         * distortionTone: Controls the harshness of the waveshaper equation, tanh ( tone * x)
         * distortionPostGain: Output gain of the distortion class
         * distortionClarity: Highpass filter cut off frequency to control low harmonics
         * distortionBypass: Bypass distortion effect
         */
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionPreGain", "distortionPreGain",
                                    juce::NormalisableRange<float>(-10.f, 100.f, 0.5f, 1.f),
                                    50.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionTone", "distortionTone",
                                    juce::NormalisableRange<float>(0.01f, 10.f, 0.5f, 1.f),
                                    5.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionPostGain", "distortionPostGain",
                                    juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                    0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionClarity", "distortionClarity",
                                    juce::NormalisableRange<float>(0.f, 5000.f, 0.5f, 1.f),
                                    1000.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>("distortionBypass", "distortionBypass", false));

        /* Amp parameters
         * ampInputGain: Initial amp gain in dBs
         * ampLowEnd: Low values attenuate low frequencies.
         * ampMids: Mid frequency gain. < 5 attenuates mid frequncies, > 5 boosts mids.
         * ampHighEnd: Low values attenuate high frequencies.
         * ampBypass: Bypass amp simulator
         */
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampInputGain", "ampInputGain",
                                    juce::NormalisableRange<float>(0.f, 100.f, 0.1f, 1.f),
                                    1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampLowEnd", "ampLowEnd",
                                    juce::NormalisableRange<float>(0.f, 10.f, 0.1f, 1.f),
                                    10.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampMids", "ampMids",
                                    juce::NormalisableRange<float>(0.f, 10.f, 0.1f, 1.f),
                                    5.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("ampHighEnd", "ampHighEnd",
                                    juce::NormalisableRange<float>(0.f, 10.f, 0.1f, 1.f),
                                    10.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>("ampBypass", "ampBypass", false));

        /* Delay parameters
         * delayTime: Amount of time between current sample and the delayed sample added to the signal
         * delayWetLevel: Determines ratio of clean signal and delayed signal
         * delayFeedback: Controls the decay time of the wet signal
         * delayBypass: Bypass the delay effect
         */
        layout.add(std::make_unique<juce::AudioParameterFloat>("delayTime", "delayTime",
                                    juce::NormalisableRange<float>(0.f, MAX_DELAY_TIME, (MAX_DELAY_TIME / 10), 1.f),
                                    0.2f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("delayWetLevel", "delayWetLevel",
                                    juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f),
                                    0.4f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("delayFeedback", "delayFeedback",
                                    juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f),
                                    0.1f));
        layout.add(std::make_unique<juce::AudioParameterBool>("delayBypass", "delayBypass", false));

        /* Reverb parameters
         * reverbIntensity: Dampening of the reverb. 0 is fully dampened.
         * reverbRoomSize: Sets reverb room size.
         * reverbWetMix: Changes the ratio of wet and dry. 1 is all wet 0 is all dry.
         * reverbSpread: Sets the spread. 1 is high.
         * reverbShimmer: Enable feedback mode.
         * reverbBypass: bypass the reverb effect
         */
        layout.add(std::make_unique<juce::AudioParameterFloat>("reverbIntensity", "reverbIntensity",
                                    juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f),
                                    0.5f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("reverbRoomSize", "reverbRoomSize",
                                    juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f),
                                    0.5f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("reverbWetMix", "reverbWetMix",
                                    juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f),
                                    0.33f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("reverbSpread", "reverbSpread",
                                    juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f),
                                    1.f));
        layout.add(std::make_unique<juce::AudioParameterBool>("reverbShimmer", "reverbShimmer", true));
        layout.add(std::make_unique<juce::AudioParameterBool>("reverbBypass", "reverbBypass", false));

        layout.add(std::make_unique<juce::AudioParameterFloat>("noiseGate", "noiseGate",
                                    juce::NormalisableRange<float>(100.f, 20000.f, 0.5f, 1.f),
                                    17500.0f));


        return layout;
    }

void PixelDriveAudioProcessor::updateNoiseGate(FilterChain& cutChain, float cutoffFreq, double sampleRate) {
    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(cutoffFreq,
                                                                                                      sampleRate,
                                                                                                      8);
    *cutChain.template get<0>().coefficients = *cutCoefficients[0];
    *cutChain.template get<1>().coefficients = *cutCoefficients[1];
    *cutChain.template get<2>().coefficients = *cutCoefficients[2];
    *cutChain.template get<3>().coefficients = *cutCoefficients[3];
}

void PixelDriveAudioProcessor::updateParameters() {
    auto chainSettings = getChainSettings(apvts);

    auto& leftPreGain = leftChain.template get<ChainPositions::preGainIndex>();
    auto& rightPreGain = rightChain.template get<ChainPositions::preGainIndex>();

    leftPreGain.setGainDecibels(chainSettings.preGain);
    rightPreGain.setGainDecibels(chainSettings.preGain);

    auto& leftDistortion = leftChain.template get<ChainPositions::distortionIndex>();
    auto& rightDistortion = rightChain.template get<ChainPositions::distortionIndex>();

    leftDistortion.setParams(chainSettings, getSampleRate());
    rightDistortion.setParams(chainSettings, getSampleRate());
    // Bypass distortion
    leftChain.setBypassed<ChainPositions::distortionIndex> (chainSettings.distortionBypass);
    rightChain.setBypassed<ChainPositions::distortionIndex> (chainSettings.distortionBypass);

    auto& leftAmpSim = leftChain.template get<ChainPositions::ampSimIndex>();
    auto& rightAmpSim = rightChain.template get<ChainPositions::ampSimIndex>();

    leftAmpSim.setParams(chainSettings, getSampleRate());
    rightAmpSim.setParams(chainSettings, getSampleRate());
    // // Bypass amp sim
    leftChain.setBypassed<ChainPositions::ampSimIndex> (chainSettings.ampBypass);
    rightChain.setBypassed<ChainPositions::ampSimIndex> (chainSettings.ampBypass);

    auto& leftDelay = leftChain.template get<ChainPositions::delayIndex>();
    auto& rightDelay = rightChain.template get<ChainPositions::delayIndex>();

    leftDelay.setParams(chainSettings, 0);
    rightDelay.setParams(chainSettings, 0);
    // Bypass delay
    leftChain.setBypassed<ChainPositions::delayIndex> (chainSettings.delayBypass);
    rightChain.setBypassed<ChainPositions::delayIndex> (chainSettings.delayBypass);

    auto& leftReverb = leftChain.template get<ChainPositions::reverbIndex>();
    auto& rightReverb = rightChain.template get<ChainPositions::reverbIndex>();

    leftReverb.setParams(chainSettings);
    rightReverb.setParams(chainSettings);
    // Bypass reverb
    leftChain.setBypassed<ChainPositions::reverbIndex> (chainSettings.reverbBypass);
    rightChain.setBypassed<ChainPositions::reverbIndex> (chainSettings.reverbBypass);

    auto& leftNoiseGate = leftChain.template get<ChainPositions::noiseGateIndex>();
    auto& rightNoiseGate = rightChain.template get<ChainPositions::noiseGateIndex>();

    updateNoiseGate(leftNoiseGate, chainSettings.noiseGate, getSampleRate());
    updateNoiseGate(rightNoiseGate, chainSettings.noiseGate, getSampleRate());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PixelDriveAudioProcessor();
}
