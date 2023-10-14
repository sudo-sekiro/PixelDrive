#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PixelDriveAudioProcessor::PixelDriveAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

PixelDriveAudioProcessor::~PixelDriveAudioProcessor()
{
}

//==============================================================================
const juce::String PixelDriveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PixelDriveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PixelDriveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PixelDriveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PixelDriveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PixelDriveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PixelDriveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PixelDriveAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PixelDriveAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PixelDriveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PixelDriveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;

    spec.sampleRate = sampleRate;

    auto& leftnoiseGate = leftChain.get<ChainPositions::noiseGateIndex>();
    leftnoiseGate.state = FilterCoefs::makeFirstOrderLowPass (spec.sampleRate, 10000.0f);

    auto& rightNoiseGate = rightChain.get<ChainPositions::noiseGateIndex>();
    rightNoiseGate.state = FilterCoefs::makeFirstOrderLowPass (spec.sampleRate, 10000.0f);

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    // Set initial plugin gain
    auto& leftPreGain = leftChain.template get<ChainPositions::preGainIndex>();
    auto& rightPreGain = rightChain.template get<ChainPositions::preGainIndex>();

    auto chainSettings = getChainSettings(apvts);

    leftPreGain.setGainDecibels (chainSettings.preGain);
    rightPreGain.setGainDecibels (chainSettings.preGain);
}

void PixelDriveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PixelDriveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void PixelDriveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

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
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    juce::dsp::AudioBlock<float> block(buffer);

    auto LeftBlock = block.getSingleChannelBlock(0);
    auto RightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(LeftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(RightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    auto& leftPreGain = leftChain.template get<ChainPositions::preGainIndex>();
    auto& rightPreGain = rightChain.template get<ChainPositions::preGainIndex>();

    auto chainSettings = getChainSettings(apvts);

    leftPreGain.setGainDecibels (chainSettings.preGain);
    rightPreGain.setGainDecibels (chainSettings.preGain);

    auto& leftDistortion = leftChain.template get<ChainPositions::distortionIndex>();
    auto& rightDistortion = rightChain.template get<ChainPositions::distortionIndex>();

    leftDistortion.setParams(chainSettings, getSampleRate());
    rightDistortion.setParams(chainSettings, getSampleRate());

    auto& leftNoiseGate = leftChain.template get<ChainPositions::noiseGateIndex>();
    auto& rightNoiseGate = rightChain.template get<ChainPositions::noiseGateIndex>();

    leftNoiseGate.state = FilterCoefs::makeFirstOrderLowPass (getSampleRate(), chainSettings.noiseGate);
    rightNoiseGate.state = FilterCoefs::makeFirstOrderLowPass (getSampleRate(), chainSettings.noiseGate);
}

//==============================================================================
bool PixelDriveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PixelDriveAudioProcessor::createEditor()
{
    // return new PixelDriveAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void PixelDriveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PixelDriveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

// Add parameter
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.preGain = apvts.getRawParameterValue("preGain")->load();

    // Return distortion parameters
    settings.distortionPreGain = apvts.getRawParameterValue("distortionPreGain")->load();
    settings.distortionTone = apvts.getRawParameterValue("distortionTone")->load();
    settings.distortionPostGain = apvts.getRawParameterValue("distortionPostGain")->load();
    settings.distortionClarity = apvts.getRawParameterValue("distortionClarity")->load();

    settings.noiseGate = apvts.getRawParameterValue("noiseGate")->load();

    return settings;
}

// Add parameters
juce::AudioProcessorValueTreeState::ParameterLayout
    PixelDriveAudioProcessor::createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterFloat>("preGain","preGain",
                                    juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                    0.0f));
        /* Distortion parameters
         * distortionPreGain: Input gain to the distortion class
         * distortionTone: Controls the harshness of the waveshaper equation, tanh ( tone * x)
         * distortionPostGain: Output gain of the distortion class
         * distortionClarity: highpass filter cut off frequency to control low harmonics
         */
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionPreGain","distortionPreGain",
                                    juce::NormalisableRange<float>(-10.f, 100.f, 0.5f, 1.f),
                                    50.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionTone","distortionTone",
                                    juce::NormalisableRange<float>(0.01f, 100.f, 0.5f, 1.f),
                                    5.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionPostGain","distortionPostGain",
                                    juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                    0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("distortionClarity","distortionClarity",
                                    juce::NormalisableRange<float>(0.f, 5000.f, 0.5f, 1.f),
                                    1000.0f));

        layout.add(std::make_unique<juce::AudioParameterFloat>("noiseGate","noiseGate",
                                    juce::NormalisableRange<float>(10000.f, 20000.f, 0.5f, 1.f),
                                    17500.0f));


        return layout;
    }

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PixelDriveAudioProcessor();
}
