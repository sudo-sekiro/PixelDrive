//==============================================================================
template <typename Type>
class CabSimulator
{
public:
    //==============================================================================
    CabSimulator()
    {
        auto dir = juce::File::getCurrentWorkingDirectory();

        int numTries = 0;

        while (! dir.getChildFile ("Resources").exists() && numTries++ < 15)
            dir = dir.getParentDirectory();

        auto impulseFile = dir.getChildFile ("Resources").getChildFile ("thrash_amp.wav");// ("guitar_amp.wav");
        assert(impulseFile.existsAsFile());

        convolution.loadImpulseResponse (impulseFile,
                                         juce::dsp::Convolution::Stereo::no,
                                         juce::dsp::Convolution::Trim::no,
                                         1024,
                                         juce::dsp::Convolution::Normalise::yes);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        convolution.prepare(spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        convolution.process(context);
    }

    //==============================================================================
    void reset() noexcept
    {
        convolution.reset();
    }

private:
    //==============================================================================

    juce::dsp::Convolution convolution{juce::dsp::Convolution::Latency{ 10 }};
};